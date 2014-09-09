#include "leagues.h"

#include <QDebug>

Leagues::Leagues() {}

void Leagues::addLeague(const QString& name) {
    leagues.push_back(name);
    lines.push_back(QVector <Line>());
}

void Leagues::addLine(int t, const QString& name1, const QString& name2, double win1, double draw, double win2) {
    lines.back().push_back(Line(t, name1, name2, win1, draw, win2));
}


void Leagues::setNumberOfLeagues(int s) {
    leags = s;
}

int Leagues::size() {
    return lines.size();
}

QString Leagues::toXml() {
    QDomDocument doc("leonbets");
    QDomElement domEl = doc.createElement("leagues");
    QDomAttr at = doc.createAttribute("number");
    at.setValue(QString().setNum(leags));
    domEl.setAttributeNode(at);

    doc.appendChild(domEl);
    for (int i = 0; i < leagues.size(); ++i) {
        QDomElement leag = doc.createElement("league");

        QDomAttr domAttrName = doc.createAttribute("name");
        domAttrName.setValue(leagues[i]);
        leag.setAttributeNode(domAttrName);

        QDomAttr id = doc.createAttribute("id");
        id.setValue(QString().setNum(i));
        leag.setAttributeNode(id);

        domEl.appendChild(leag);
        for (int j = 0; j < lines[i].size(); ++j) {
            QDomElement line = doc.createElement("line");
            leag.appendChild(line);
            lines[i][j].addSelfLikeSon(doc, line);
        }
    }
    return doc.toString();
}
