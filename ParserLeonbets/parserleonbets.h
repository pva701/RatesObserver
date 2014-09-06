#ifndef PARSERLEONBETS_H
#define PARSERLEONBETS_H

#include <QWebElement>
#include <QWebFrame>
#include <QWebPage>
#include <QUrl>
#include "leagues.h"

#include <iostream>
using namespace std;

namespace LineExtracters {
    int extractTime(const QWebElement& element);
    QString extractComands(const QWebElement& element);
    void trim(QString& comand);
    QString extractFirstComand(const QWebElement& element);
    QString extractSecondComand(const QWebElement& element);
    double extractWin1(const QWebElement& element);
    double extractDraw(const QWebElement &element);
    double extractWin2(const QWebElement &element);
}


class ParserLeonbets:public QObject
{
    Q_OBJECT

    QWebPage mainPage;
    void examineChildElements(int id, const QWebElement &parentElement);
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
signals:
    void loadFinishedMainPage(bool);
private slots:
    void slotLoadMainPageFinished(bool status);
    void printProgress(int percent);
    void slotLoadLeaguePageFinished(bool);
};

#endif // PARSERLEONBETS_H
