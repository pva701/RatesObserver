#include "parserleonbets.h"

#include <QObject>
#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QTimer>
#include "leagues.h"

//namespace LineExtracters {

ParserLeonbets::ParserLeonbets():
    url(QUrl::fromUserInput(QString::fromLatin1("ru.leonbets.net"))), timeMatch(2e9) {
    loadedPercent = 0;
}

void ParserLeonbets::parseLeague(int id, const QWebElement &v) {
    if (v.tagName().toLower() == "tr" && (v.hasClass("row1") || v.hasClass("row2"))) {
        QWebElement vnx = v.nextSibling();
        if (vnx.tagName().toLower() == "tr")
            collect.addLine(Leagues::Line(v, vnx));
        else
            collect.addLine(Leagues::Line(v));
        return;
    }

    QWebElement element = v.firstChild();
    while (!element.isNull()) {
        parseLeague(id, element);
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
    parseLeague(collect.size() - 1, league->mainFrame()->documentElement());

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
