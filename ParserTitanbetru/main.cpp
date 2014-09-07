#include <QtGui/QApplication>
#include "parsertitanbetru.h"
#include <cstdio>
#include <QVector>
#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QDebug>
using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ParserTitanbetru parser;
    parser.parse();
    return a.exec();
}
