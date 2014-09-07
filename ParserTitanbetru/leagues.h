#ifndef LEAGUES_H
#define LEAGUES_H

#include <QString>
#include <QVector>
#include <QDomElement>
#include <QDomDocument>

class Leagues
{
    struct Line {
        int t;
        QString name1, name2;
        double win1, win2, draw;
        Line() {}
        Line(int tt, const QString& n1, const QString& n2, double w1, double dr, double w2) {
            t = tt;
            name1 = n1;
            name2 = n2;
            win1 = w1;
            draw = dr;
            win2 = w2;
        }

        void addSelfLikeSon(QDomDocument& doc, QDomElement& el) {
            append(doc, el, "time", QString().setNum(t));
            append(doc, el, "team1", name1);
            append(doc, el, "team2", name2);
            append(doc, el, "win1", QString().setNum(win1, 'g', 2));
            append(doc, el, "draw", QString().setNum(draw, 'g', 2));
            append(doc, el, "win2", QString().setNum(win2, 'g', 2));
        }

        void append(QDomDocument& doc, QDomElement& el, const QString& nm, const QString& val) {
            QDomElement tNode = doc.createElement(nm);
            tNode.appendChild(doc.createTextNode(val));
            el.appendChild(tNode);
        }
    };

    QVector <QString> leagues;
    QVector <QVector <Line> > lines;
    int leags;
public:
    Leagues();
    void addLeague(const QString& name);
    void addLine(int t, const QString& name1, const QString& name2,
                 double win1, double draw, double win2);
    QString toXml();
    void setNumberOfLeagues(int s);
    int size();
    void exportFromXml();
};

#endif // LEAGUES_H
