#include "leagues.h"

#include <QDebug>

Leagues::Leagues() {}

void Leagues::addLeague(const QString& name) {
    leagues.push_back(name);
    lines.push_back(QVector <Line>());
}

void Leagues::addLine(int id, int t, const QString& name1, const QString& name2, double win1, double draw, double win2) {
    lines[id].push_back(Line(t, name1, name2, win1, draw, win2));
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

        QDomAttr domAttrId = doc.createAttribute("id");
        domAttrId.setValue(QString().setNum(i));
        leag.setAttributeNode(domAttrId);

        QDomAttr domAttrName = doc.createAttribute("name");
        domAttrName.setValue(leagues[i]);
        leag.setAttributeNode(domAttrName);

        domEl.appendChild(leag);
        for (int j = 0; j < lines[i].size(); ++j) {
            QDomElement line = doc.createElement("line");
            QDomAttr domAttr = doc.createAttribute("id");
            domAttr.setValue(QString().setNum(j));
            line.setAttributeNode(domAttr);
            leag.appendChild(line);
            lines[i][j].addSelfLikeSon(doc, line);
        }
    }
    return doc.toString();
}
//https://vk.com/audios-5316256
