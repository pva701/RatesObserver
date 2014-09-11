#include "leagues.h"

#include <QDebug>
#include <QWebElement>

int LineExtracters::extractTime(const QWebElement& element) {
    QString s = element.firstChild().firstChild().toPlainText();
    int l = s.indexOf("(");
    int r = s.indexOf(")");
    return s.mid(l + 1, r - l - 1).toLongLong() / 1000;
}

QString LineExtracters::extractTeams(const QWebElement& element) {
    return element.firstChild().nextSibling().firstChild().firstChild().toPlainText();
}

void LineExtracters::trim(QString& comand) {
    int i = 0;
    while (comand[i] == ' ') ++i;
    comand.remove(0, i);
    i = comand.size() - 1;
    while (comand[i] == ' ') --i;
    comand.remove(i + 1, comand.size() - i - 1);
}

QString LineExtracters::extractFirstTeam(const QWebElement& element) {
    QString s = extractTeams(element);
    int pos = s.indexOf(" - ");
    QString first = s.mid(0, pos);
    trim(first);
    return first;
}

QString LineExtracters::extractSecondTeam(const QWebElement& element) {
    QString s = extractTeams(element);
    int pos = s.indexOf(" - ");
    QString second = s.mid(pos + 3, s.size() - pos - 1);
    trim(second);
    return second;
}

double LineExtracters::extractWin1(const QWebElement& element) {
    return element.firstChild().nextSibling().nextSibling().firstChild().firstChild().toPlainText().toDouble();
}

double LineExtracters::extractDraw(const QWebElement &element) {
    return element.firstChild().nextSibling().nextSibling().nextSibling().firstChild().firstChild().toPlainText().toDouble();
}

double LineExtracters::extractWin2(const QWebElement &element) {
    return element.firstChild().nextSibling().nextSibling().nextSibling().nextSibling().firstChild().firstChild().toPlainText().toDouble();
}
using namespace LineExtracters;

Leagues::Line::Line(const QWebElement& element) {
    t = extractTime(element) + 4 * 3600;
    name1 = extractFirstTeam(element);
    name2 = extractSecondTeam(element),
    win1 = extractWin1(element);
    draw = extractDraw(element);
    win2 = extractWin2(element);
}

Leagues::Line::Line(const QWebElement& element, const QWebElement& extrTbl) {
    t = extractTime(element) + 4 * 3600;
    name1 = extractFirstTeam(element);
    name2 = extractSecondTeam(element),
    win1 = extractWin1(element);
    draw = extractDraw(element);
    win2 = extractWin2(element);
    QWebElementCollection strongs = extrTbl.findAll("strong");
    int c = strongs.count();
    win1draw = strongs[1].toPlainText().toDouble();
    win1win2 = strongs[2].toPlainText().toDouble();
    win2draw = strongs[3].toPlainText().toDouble();
    for (int i = 4; i < c; ++i)
        if (strongs[i].toPlainText().indexOf(QObject::trUtf8("Б/М")) != -1) {
            tot.push_back(strongs[i].toPlainText().remove(0, 11).mid(0, 3).toDouble());
            under.push_back(strongs[i + 1].toPlainText().toDouble());
            over.push_back(strongs[i + 2].toPlainText().toDouble());
            i += 2;
        }
}

void Leagues::Line::addSelfLikeSon(QDomDocument& doc, QDomElement& el) {
    append(doc, el, "time", QString().setNum(t));
    append(doc, el, "team1", name1);
    append(doc, el, "team2", name2);
    append(doc, el, "win1", QString().setNum(win1, 'g', 2));
    append(doc, el, "draw", QString().setNum(draw, 'g', 2));
    append(doc, el, "win2", QString().setNum(win2, 'g', 2));

    QDomElement table = doc.createElement("table");
    for (int i = 0; i < tot.size(); ++i) {
        QDomElement item = doc.createElement("item");
        item.setAttribute("tot", QString().setNum(tot[i]));
        item.setAttribute("under", QString().setNum(under[i]));
        item.setAttribute("over", QString().setNum(over[i]));
        table.appendChild(item);
    }
    el.appendChild(table);
}

void Leagues::Line::append(QDomDocument& doc, QDomElement& el, const QString& nm, const QString& val) {
    QDomElement tNode = doc.createElement(nm);
    tNode.appendChild(doc.createTextNode(val));
    el.appendChild(tNode);
}



Leagues::Leagues() {}

void Leagues::addLeague(const QString& name) {
    leagues.push_back(name);
    lines.push_back(QVector <Line>());
}

void Leagues::addLine(const Line& l) {
    lines.back().push_back(l);
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
    //QDomAttr at = doc.createAttribute("number");
    //at.setValue(QString().setNum(leags));
    domEl.setAttribute("number", QString().setNum(leags));

    doc.appendChild(domEl);
    for (int i = 0; i < leagues.size(); ++i) {
        QDomElement leag = doc.createElement("league");

        QDomAttr domAttrName = doc.createAttribute("name");
        domAttrName.setValue(leagues[i]);
        leag.setAttributeNode(domAttrName);

        //QDomAttr id = doc.createAttribute("id");
        //id.setValue(QString().setNum(i));
        leag.setAttribute("id", QString().setNum(i));

        domEl.appendChild(leag);
        for (int j = 0; j < lines[i].size(); ++j) {
            QDomElement line = doc.createElement("line");

            leag.appendChild(line);
            lines[i][j].addSelfLikeSon(doc, line);
        }
    }
    return doc.toString();
}
