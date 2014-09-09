#include "parserleonbets.h"

#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QTimer>

//namespace LineExtracters {
int LineExtracters::extractTime(const QWebElement& element) {
    QString s = element.firstChild().firstChild().toPlainText();
    int l = s.indexOf("(");
    int r = s.indexOf(")");
    return s.mid(l + 1, r - l - 1).toLongLong() / 1000;
}

QString LineExtracters::extractTeams(const QWebElement& element) {
    return element.firstChild().nextSibling().firstChild().firstChild().toPlainText();
}

void LineExtracters::trim(QString& comand) {
    int i = 0;
    while (comand[i] == ' ') ++i;
    comand.remove(0, i);
    i = comand.size() - 1;
    while (comand[i] == ' ') --i;
    comand.remove(i + 1, comand.size() - i - 1);
}

QString LineExtracters::extractFirstTeam(const QWebElement& element) {
    QString s = extractTeams(element);
    int pos = s.indexOf(" - ");
    QString first = s.mid(0, pos);
    trim(first);
    return first;
}

QString LineExtracters::extractSecondTeam(const QWebElement& element) {
    QString s = extractTeams(element);
    int pos = s.indexOf(" - ");
    QString second = s.mid(pos + 1, s.size() - pos - 1);
    trim(second);
    return second;
}

double LineExtracters::extractWin1(const QWebElement& element) {
    return element.firstChild().nextSibling().nextSibling().firstChild().firstChild().toPlainText().toDouble();
}

double LineExtracters::extractDraw(const QWebElement &element) {
    return element.firstChild().nextSibling().nextSibling().nextSibling().firstChild().firstChild().toPlainText().toDouble();
}

double LineExtracters::extractWin2(const QWebElement &element) {
    return element.firstChild().nextSibling().nextSibling().nextSibling().nextSibling().firstChild().firstChild().toPlainText().toDouble();
}
using namespace LineExtracters;

ParserLeonbets::ParserLeonbets():
    url(QUrl::fromUserInput(QString::fromLatin1("ru.leonbets.net"))), timeMatch(2e9) {
    loadedPercent = 0;
}

void ParserLeonbets::examineChildElements(int id, const QWebElement &parentElement) {
    QWebElement element = parentElement.firstChild();
    while (!element.isNull()) {
        if (element.tagName().toLower() == "tr") {
            if (element.hasClass("row1") || element.hasClass("row2")) {
                int t = extractTime(element);
                //if (t <= timeMatch)
                collect.addLine(t, extractFirstTeam(element), extractSecondTeam(element),
                                extractWin1(element), extractDraw(element), extractWin2(element));
            }
        }
        examineChildElements(id, element);
        element = element.nextSibling();
    }
}

void ParserLeonbets::findLeagueLinks(const QWebElement &parentElement) {
    if (links.size())
        return;
    QWebElement element = parentElement.firstChild();
    //qDebug() << parentElement.tagName() << " " << parentElement.isNull();
    while (!element.isNull()) {
        if (element.tagName().toLower() == "div" && element.attribute("id") == "lg1") {
            QWebElement div = element.firstChild().firstChild();
            while (!div.isNull()) {
                links.push_back(make_pair(div.firstChild().toPlainText(), div.firstChild().attribute("href")));
                div = div.nextSibling();
            }
        }
        findLeagueLinks(element);
        element = element.nextSibling();
    }
}

void ParserLeonbets::lockAttributs(QWebPage& page) {
    QWebSettings *settings = page.settings();
    settings->setAttribute(QWebSettings::JavascriptEnabled, false);
    settings->setAttribute(QWebSettings::PluginsEnabled, false);
    settings->setAttribute(QWebSettings::AutoLoadImages, false);
    settings->setAttribute(QWebSettings::LinksIncludedInFocusChain, false);///
    settings->setAttribute(QWebSettings::PrintElementBackgrounds, false);
    settings->setAttribute(QWebSettings::AcceleratedCompositingEnabled, false);//shit
    settings->setAttribute(QWebSettings::SiteSpecificQuirksEnabled, false);
}

void ParserLeonbets::setTime(int t) {
    timeMatch = t;
}

void ParserLeonbets::parse() {
    startParse = clock();
    mainPage = new QWebPage();
    connect(mainPage, SIGNAL(loadFinished(bool)), this, SLOT(slotLoadMainPageFinished(bool)));
    connect(mainPage, SIGNAL(loadProgress(int)), this, SLOT(printProgress(int)));
    lockAttributs(*mainPage);
    mainPage->mainFrame()->load(url);
}

void ParserLeonbets::printProgress(int percent) {
    if (loadedPercent >= percent)
        return;

    while (loadedPercent++ < percent)
        std::cout << "#" << std::flush;
    cout << percent << std::flush;
}

void ParserLeonbets::slotLoadMainPageFinished(bool status) {
    if (!status) {
        cout << "\nerror load main page" << endl;
        return;
    }
    cout << endl;
    //qDebug() << mainPage->mainFrame()->toHtml();
    findLeagueLinks(mainPage->mainFrame()->documentElement());
    if (links.back().second == "")
        links.pop_back();
    qDebug() << "total leags = " << links.size();
    collect.setNumberOfLeagues(links.size());

    league = new QWebPage();
    lockAttributs(*league);
    numberOfLink = 0;
    connect(league, SIGNAL(loadFinished(bool)), this, SLOT(slotLoadLeaguePageFinished(bool)), Qt::DirectConnection);
    league->mainFrame()->load(links[0].second);
}

void ParserLeonbets::slotLoadLeaguePageFinished(bool status) {
    if (!status) {
        qDebug() << "error load league page" << endl;
        return;
    }
    cout << "succ leag " << numberOfLink + 1 << endl;
    collect.addLeague(links[numberOfLink].first);
    examineChildElements(collect.size() - 1, league->mainFrame()->documentElement());

    QFile file("leonbets.xml");
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream(&file) << collect.toXml();
        file.close();
    }
    ++numberOfLink;
    if (numberOfLink < links.size())
        league->mainFrame()->load(links[numberOfLink].second);
    else {
        cout << "time of work = " << (clock() - startParse) / 100000.0 << " sec.\n";
        exit(0);
    }
}
