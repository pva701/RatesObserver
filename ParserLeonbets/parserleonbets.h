#ifndef PARSERLEONBETS_H
#define PARSERLEONBETS_H

#include <QWebElement>
#include <QWebFrame>
#include <QWebPage>

#include <iostream>
using namespace std;

class ParserLeonbets:public QObject
{
    Q_OBJECT

    QWebPage mainPage;
    void examineChildElements(const QWebElement &parentElement);
    void findLeagueLinks(const QWebElement &parentElement);
    void lockAttributs(QWebPage& page);

    QUrl url;
    int loadedPercent;
    long long startParse;
    QVector <QString> links;
public:
    ParserLeonbets();
    void parse();
    void setUrl(const QUrl& ur);
signals:
    void loadFinishedMainPage(bool);
private slots:
    void slotLoadMainPageFinished(bool status);
    void printProgress(int percent);
    void slotLoadLeaguePageFinished(bool);
};

#endif // PARSERLEONBETS_H
