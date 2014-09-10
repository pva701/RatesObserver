#include "parsertitanbetru.h"
#include <QUrl>
#include <QDebug>
#include <QFile>
#include <QDateTime>

ParserTitanbetru::ParserTitanbetru():numberOfLink(0) {
}

void ParserTitanbetru::parse() {
    QUrl url(QUrl::fromUserInput(QString::fromLatin1(URL)));
    mainPage = new QWebPage();
    lockAttributs(*mainPage);
    startParse = clock();
    connect(mainPage, SIGNAL(loadFinished(bool)), this, SLOT(slotLoadMainPageFinished(bool)));
    mainPage->mainFrame()->load(url);
}

void ParserTitanbetru::lockAttributs(QWebPage& page) {
    QWebSettings *settings = page.settings();
    settings->setAttribute(QWebSettings::JavascriptEnabled, false);
    settings->setAttribute(QWebSettings::PluginsEnabled, false);
    settings->setAttribute(QWebSettings::AutoLoadImages, false);
    settings->setAttribute(QWebSettings::LinksIncludedInFocusChain, false);
    settings->setAttribute(QWebSettings::PrintElementBackgrounds, false);
    settings->setAttribute(QWebSettings::AcceleratedCompositingEnabled, false);
    settings->setAttribute(QWebSettings::SiteSpecificQuirksEnabled, false);
}

QString ParserTitanbetru::getLink() {

    QString s;
    for (int i = numberOfLink; i < std::min(numberOfLink + 5, (int)links.size()); ++i)
        s += links[i] + "-";
    s = s.remove(s.size() - 1, 1);
    numberOfLink = std::min(numberOfLink + 5, (int)links.size());
    return "http://sports.titanbetru.com/ru/type-coupon?sb_type_ids=" + s;
}

int ParserTitanbetru::getGMT() {
    QWebElement el = mainPage->mainFrame()->documentElement().findFirst("div[class=\"header-dropdown\"]");
    el = el.findFirst("option[selected=\"selected\"]");
    return el.toPlainText().remove(0, 4).toInt();
}

void ParserTitanbetru::slotLoadMainPageFinished(bool status) {
    if (!status) {
        qDebug() << "error load main page\n";
        return;
    }
    qDebug() << "success load";
    gmt = getGMT();
    qDebug() << "GMT = " << gmt;

    QWebElement list = findByText(mainPage->mainFrame()->documentElement(), trUtf8("Футбол")).parent().nextSibling();
    extractLinks(list, links);
    league = new QWebPage();
    lockAttributs(*league);
    connect(league, SIGNAL(loadFinished(bool)), this, SLOT(slotLoadLeaguePageFinishedIterate(bool)));
    qDebug() << "total leagues = " << links.size();
    collect.setNumberOfLeagues(links.size());
    league->mainFrame()->load(getLink());
}

QString ParserTitanbetru::format(QString m) {
    const char* month1[] = {"Январь", "Февраль", "Март", "Апрель", "Май", "Июнь",
                            "Июль", "Август", "Сентябрь", "Октябрь", "Ноябрь", "Декабрь"};
    const char* month2[] = {"января", "февраля", "марта", "апреля", "мая", "июня",
                            "июля", "августа", "сентября", "октября", "ноября", "декабря"};
    for (int i = 0; i < 12; ++i)
        m = m.replace(QString::fromUtf8(month1[i]), QString::fromUtf8(month2[i]));
    return m;
}

void ParserTitanbetru::extractLine(const QWebElement& v) {
    QWebElement dtm = v.findFirst("td[class=\"time coupon-scoreboard\"]");
    QString time = dtm.firstChild().firstChild().toPlainText();
    QString date = dtm.firstChild().firstChild().nextSibling().toPlainText();
    QWebElementCollection cl = v.findAll("span[class=\"seln-name\"]");

    QString team1 = cl[0].firstChild().toPlainText();
    double win1 = cl[0].parent().firstChild().nextSibling().nextSibling().nextSibling().toPlainText().toDouble();

    double draw = cl[1].parent().firstChild().nextSibling().nextSibling().toPlainText().toDouble();

    QString team2 = cl[2].firstChild().toPlainText();
    double win2 = cl[2].parent().firstChild().nextSibling().nextSibling().nextSibling().toPlainText().toDouble();

    QDateTime d = QDateTime::fromString(time + " " + format(date) + " " +
                                      QString().setNum(QDateTime::currentDateTime().date().year()), "hh:mm dd MMMM yyyy");
    //d = d.addSecs((4 - gmt) * 3600);
    int t = d.toTime_t() + 7 * 3600;
    collect.addLine(t, team1, team2, win1, draw, win2);
}

void ParserTitanbetru::parseLeagues(const QWebElement& v) {
    if (v.tagName().toLower() == "span" && v.attribute("data-ref_key").toLower() == "type")
        collect.addLeague(v.parent().toPlainText().remove(0, 2));

    if (v.tagName().toLower() == "tr") {
        extractLine(v);
        return;
    }

    QWebElement el = v.firstChild();
    while (!el.isNull()) {
        parseLeagues(el);
        el = el.nextSibling();
    }
}

void ParserTitanbetru::slotLoadLeaguePageFinishedIterate(bool status) {
    if (!status) {
        qDebug() << "error load league page";
        return;
    }
    qDebug() << "succ load league page " << numberOfLink;
    parseLeagues(league->mainFrame()->documentElement());

    QFile file("titanbetru.xml");
    if (file.open(QIODevice::WriteOnly)) {
       QTextStream(&file) << collect.toXml();
       file.close();
    }

    if (numberOfLink < links.size())
        league->mainFrame()->load(getLink());
    else {
        qDebug() << (clock() - startParse) / 100000.0 << " sec.";
        exit(0);
    }
}

void ParserTitanbetru::extractLinks(const QWebElement& v, QVector <QString>& ids) {
    if (v.tagName().toLower() == "a" && v.hasAttribute("href")) {
        QString s = v.attribute("href");
        int i = 6;
        while (s[i] != '/') ++i;
        ids.push_back(s.mid(6, i - 6));
    }
    QWebElement el = v.firstChild();
    while (!el.isNull()) {
        extractLinks(el, ids);
        el = el.nextSibling();
    }
}

QWebElement ParserTitanbetru::findByText(const QWebElement& v, const QString& text) {
    if (v.toPlainText() == text && v.firstChild().isNull())
        return v;
    QWebElement el = v.firstChild();
    while (!el.isNull()) {
        QWebElement f = findByText(el, text);
        if (!f.isNull())
            return f;
        el = el.nextSibling();
    }
    return QWebElement();
}
