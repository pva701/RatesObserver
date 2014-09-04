#include "parserleonbets.h"

#include <QDebug>
#include <QDateTime>

namespace LineExtracters {
    int extractTime(const QWebElement& element) {
        QString s = element.firstChild().firstChild().toPlainText();
        int l = s.indexOf("(");
        int r = s.indexOf(")");
        return s.mid(l + 1, r - l - 1).toLongLong() / 1000;
    }

    QString extractComands(const QWebElement& element) {
        return element.firstChild().nextSibling().firstChild().firstChild().toPlainText();
    }

    void trim(QString& comand) {
        int i = 0;
        while (comand[i] == ' ') ++i;
        comand.remove(0, i);
        i = comand.size() - 1;
        while (comand[i] == ' ') --i;
        comand.remove(i + 1, comand.size() - i - 1);
    }

    QString extractFirstComand(const QWebElement& element) {
        QString s = extractComands(element);
        int pos = s.indexOf("-");
        QString first = s.mid(0, pos);
        trim(first);
        return first;
    }

    QString extractSecondComand(const QWebElement& element) {
        QString s = extractComands(element);
        int pos = s.indexOf("-");
        QString second = s.mid(pos + 1, s.size() - pos - 1);
        trim(second);
        return second;
    }

    double extractWin1(const QWebElement& element) {
        return element.firstChild().nextSibling().nextSibling().firstChild().firstChild().toPlainText().toDouble();
    }

    double extractDraw(const QWebElement &element) {
        return element.firstChild().nextSibling().nextSibling().nextSibling().firstChild().firstChild().toPlainText().toDouble();
    }

    double extractWin2(const QWebElement &element) {
        return element.firstChild().nextSibling().nextSibling().nextSibling().nextSibling().firstChild().firstChild().toPlainText().toDouble();
    }
}

using namespace LineExtracters;

ParserLeonbets::ParserLeonbets() {
    connect(&mainPage, SIGNAL(loadFinished(bool)), this, SLOT(slotLoadMainPageFinished(bool)));
    connect(&mainPage, SIGNAL(loadProgress(int)), this, SLOT(printProgress(int)));
    loadedPercent = 0;
}

void ParserLeonbets::examineChildElements(const QWebElement &parentElement) {
    QWebElement element = parentElement.firstChild();
    while (!element.isNull()) {
        if (element.tagName().toLower() == "tr") {
            if (element.hasClass("row1") || element.hasClass("row2")) {
                /*qDebug() << extractTime(element) << " " << extractFirstComand(element) << " " << extractSecondComand(element) << " " <<
                            extractWin1(element) << " " << extractDraw(element) << " " << extractWin2(element) << endl;*/
            }
        }
        examineChildElements(element);
        element = element.nextSibling();
    }
}

void ParserLeonbets::findLeagueLinks(const QWebElement &parentElement) {
    if (links.size())
        return;
    QWebElement element = parentElement.firstChild();
    while (!element.isNull()) {
        if (element.tagName().toLower() == "div" && element.attribute("id") == "lg1") {
            QWebElement div = element.firstChild().firstChild();
            while (!div.isNull()) {
                links.push_back(div.firstChild().attribute("href"));
                div = div.nextSibling();
            }
        }
        findLeagueLinks(element);
        element = element.nextSibling();
    }
}

void ParserLeonbets::setUrl(const QUrl& ur) {
    url = ur;
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

void ParserLeonbets::parse() {
    startParse = clock();
    lockAttributs(mainPage);
    mainPage.mainFrame()->load(url);
}

void ParserLeonbets::printProgress(int percent)
{
    if (loadedPercent >= percent)
        return;

    while (loadedPercent++ < percent)
        std::cout << "#" << std::flush;
    cout << percent << std::flush;
}

void ParserLeonbets::slotLoadMainPageFinished(bool status) {
    emit loadFinishedMainPage(status);
    if (!status) {
        cout << "\nerror load main page" << endl;
        return;
    }
    cout << "\nsuccess load! with time = " << (clock() - startParse) / 100000.0 << endl;
    findLeagueLinks(mainPage.mainFrame()->documentElement());

    if (links.back() == "")
        links.pop_back();
    qDebug() << "total leags = " << links.size();
    for (int i = 0; i < links.size(); ++i) {
        QWebPage *league = new QWebPage();
        connect(league, SIGNAL(loadFinished(bool)), this, SLOT(slotLoadLeaguePageFinished(bool)));
        lockAttributs(*league);
        league->mainFrame()->load(links[i]);
    }
}

void ParserLeonbets::slotLoadLeaguePageFinished(bool status) {
    static int suc = 0;
    if (!status) {
        qDebug() << "error load league page" << endl;
        return;
    }
    qDebug() << "succ leag " << ++suc;
    QWebPage *from = (QWebPage*)sender();
    examineChildElements(from->mainFrame()->documentElement());
}
