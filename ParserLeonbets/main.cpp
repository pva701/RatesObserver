#include <iostream>
#include <QtGui>
#include "parserleonbets.h"
using namespace std;



 int main(int argc, char * argv[])
 {
     //argc = 2;
     //argv[1] = "https://ru.leonbets.net/";
     if (argc != 2) {
         std::cout << "input url, please!\n";
         return 0;
     }

     QUrl url = QUrl::fromUserInput(QString::fromLatin1(argv[1]));
     QApplication a(argc, argv);

     ParserLeonbets parser;
     parser.setUrl(url);
     parser.parse();
     return a.exec();
 }
