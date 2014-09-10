#ifndef PARSERTITANBETRU_H
#define PARSERTITANBETRU_H
#include <QVector>

#include <QWebElement>
#include <QWebPage>
#include <QWebFrame>
#include "leagues.h"

class ParserTitanbetru: public QObject
{
    Q_OBJECT

    const char* URL = "http://sports.titanbetru.com/ru/football";
    QWebPage *mainPage;
    QWebElement findByText(const QWebElement& v, const QString& text);
    void extractLinks(const QWebElement& v, QVector <QString>& ids);
    int numberOfLink;
    QVector <QString> links;
    QWebPage *league;
    QString getLink();
    void parseLeagues(const QWebElement& v);
    Leagues collect;
    void extractLine(const QWebElement& v);
    QString format(QString m);
    int getGMT();
    int gmt;
    int startParse;
public:
    ParserTitanbetru();
    void parse();
    void lockAttributs(QWebPage& page);
private slots:
    void slotLoadMainPageFinished(bool status);
    void slotLoadLeaguePageFinishedIterate(bool status);
};

#endif // HELPERPARSERVALIDLEAGUESLINK_H
