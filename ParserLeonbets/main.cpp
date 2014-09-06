#include <QtGui>
#include "parserleonbets.h"
#include <cstdio>

int main(int argc, char * argv[]) {
    QApplication a(argc, argv);

    int t;
    if (argc != 2) {
        qDebug() << "warning: you don't input a time!\n";
        t = 2e9;
    } else
        t = QString(argv[1]).toInt();

    ParserLeonbets parser;
    parser.setTime(t);
    parser.parse();
    return a.exec();
}
