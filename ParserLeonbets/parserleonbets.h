#ifndef PARSERLEONBETS_H
#define PARSERLEONBETS_H

#include <QWebElement>
#include <QWebFrame>
#include <QWebPage>
#include <QUrl>
#include <QWebView>
#include "leagues.h"

#include <iostream>
using namespace std;



class ParserLeonbets:public QObject
{
    Q_OBJECT

    QWebPage *mainPage;
    void parseLeague(int id, const QWebElement &parentElement);
    void findLeagueLinks(const QWebElement &parentElement);
    void lockAttributs(QWebPage& page);
    const QUrl url;

    Leagues collect;
    int loadedPercent, timeMatch;
    long long startParse;
    QVector <pair <QString, QString> > links;
    QWebPage *league;
    int numberOfLink;
public:
    ParserLeonbets();
    void parse();
    void setTime(int t);
private slots:
    void slotLoadMainPageFinished(bool status);
    void printProgress(int percent);
    void slotLoadLeaguePageFinished(QString, QWebElement, bool);
    void finishedThread();
};

#endif // PARSERLEONBETS_H
