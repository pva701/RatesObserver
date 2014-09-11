#include "loader.h"
#include <QDebug>

Loader::Loader(const QVector <pair <QString, QString> >& v) {
    links = v;
    numOfLink = 0;
}

void Loader::run() {
    qDebug() << "in run in thread " << QThread::currentThreadId();
    QWebPage *loader = new QWebPage();
    loaders.push_back(loader);
    QWebSettings *settings = loader->settings();
    settings->setAttribute(QWebSettings::JavascriptEnabled, false);
    settings->setAttribute(QWebSettings::PluginsEnabled, false);
    settings->setAttribute(QWebSettings::AutoLoadImages, false);
    settings->setAttribute(QWebSettings::LinksIncludedInFocusChain, false);///
    settings->setAttribute(QWebSettings::PrintElementBackgrounds, false);
    settings->setAttribute(QWebSettings::AcceleratedCompositingEnabled, false);//shit
    settings->setAttribute(QWebSettings::SiteSpecificQuirksEnabled, false);
    connect(loader, SIGNAL(loadFinished(bool)), SLOT(slotLoadFinished(bool)));
    loader->mainFrame()->load(links[0].second);
    exec();
}

void Loader::slotLoadFinished(bool status) {
    qDebug() << "finished load " << numOfLink + 1 << " from " << links.size() << " in thread " << QThread::currentThreadId();
    //emit finishedThreadLoad(links[numOfLink].first, loader->mainFrame()->documentElement(), status);
    ++numOfLink;
    if (numOfLink < links.size()) {
        QWebPage *loader = new QWebPage();
        loaders.push_back(loader);
        QWebSettings *settings = loader->settings();
        settings->setAttribute(QWebSettings::JavascriptEnabled, false);
        settings->setAttribute(QWebSettings::PluginsEnabled, false);
        settings->setAttribute(QWebSettings::AutoLoadImages, false);
        settings->setAttribute(QWebSettings::LinksIncludedInFocusChain, false);///
        settings->setAttribute(QWebSettings::PrintElementBackgrounds, false);
        settings->setAttribute(QWebSettings::AcceleratedCompositingEnabled, false);//shit
        settings->setAttribute(QWebSettings::SiteSpecificQuirksEnabled, false);
        connect(loader, SIGNAL(loadFinished(bool)), SLOT(slotLoadFinished(bool)), Qt::QueuedConnection);
        loader->mainFrame()->load(links[numOfLink].second);
    } else
        exit(0);
}
