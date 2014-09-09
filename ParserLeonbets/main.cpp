#include <QtGui>
#include "parserleonbets.h"
#include <cstdio>

int main(int argc, char * argv[]) {
    QApplication a(argc, argv);

    ParserLeonbets parser;
    parser.parse();
    return a.exec();
}
