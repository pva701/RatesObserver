#ifndef LOADER_H
#define LOADER_H
#include <QThread>
#include <QWebPage>
#include <QWebFrame>
#include <QWebElement>
using namespace std;

class Loader:public QThread
{
    Q_OBJECT
    //QWebPage *loader;
    int numOfLink;
public:
    QVector <std::pair <QString, QString> > links;
    QVector <QWebPage*> loaders;

    Loader(const QVector <pair <QString, QString> >& vec);
    void run();
private slots:
    void slotLoadFinished(bool);
signals:
    void finishedThreadLoad(QString, QWebElement, bool);
};

#endif // LOADER_H
