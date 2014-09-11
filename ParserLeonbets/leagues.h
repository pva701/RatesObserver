#ifndef LEAGUES_H
#define LEAGUES_H

#include <QString>
#include <QVector>
#include <QDomElement>
#include <QDomDocument>
#include <QWebElement>

namespace LineExtracters {
    int extractTime(const QWebElement& element);
    QString extractTeams(const QWebElement& element);
    void trim(QString& comand);
    QString extractFirstTeam(const QWebElement& element);
    QString extractSecondTeam(const QWebElement& element);
    double extractWin1(const QWebElement& element);
    double extractDraw(const QWebElement &element);
    double extractWin2(const QWebElement &element);
}

struct Leagues
{
    struct Line {
        int t;
        QString name1, name2;
        double win1, win2, draw, win1draw, win1win2, win2draw;
        QVector <double> tot, under, over;
        Line() {}
        Line(const QWebElement& el);
        Line(const QWebElement& el, const QWebElement& extrTbl);
        //Line(int tt, const QString& n1, const QString& n2, double w1, double dr, double w2);
        void addSelfLikeSon(QDomDocument& doc, QDomElement& el);
        void append(QDomDocument& doc, QDomElement& el, const QString& nm, const QString& val);
    };

    Leagues();
    void addLeague(const QString& name);
    void addLine(const Line& l);
    QString toXml();
    void setNumberOfLeagues(int s);
    int size();
    void exportFromXml();
private:
    QVector <QString> leagues;
    QVector <QVector <Line> > lines;
    int leags;
};

#endif // LEAGUES_H
