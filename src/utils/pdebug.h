/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */
#ifndef PDEBUG

#include <QDebug>
#include <QRegExp>
#include <QString>
#include <QThread>
#include <QTime>

#define PDEBUG_METHOD_STRING \
        qPrintable(QString(__PRETTY_FUNCTION__) \
                .replace(QRegExp("^[a-zA-Z]*::[a-zA-Z*<>]*\\s"),"") \
                .replace(QRegExp("^[a-zA-Z*<>]*\\s"),"") \
                .replace(QRegExp("\\(.*\\)"), "()") \
                .replace(QRegExp("^void "), "") \
                )

#define PDEBUG \
        qDebug() << PDEBUG_METHOD_STRING

#define PDEBUGMEM \
        qDebug() << PDEBUG_METHOD_STRING << pDebug::memory()

namespace pDebug {
    QString memory();
}

#define INITIMING \
    QTime inittime = QTime::currentTime(); \
    QTime relatime = QTime::currentTime();

#define PRINTIMING(x) \
    PDEBUG << QThread::currentThread() << (x) << "line:" << __LINE__ << inittime.msecsTo(QTime::currentTime()) << relatime.msecsTo(QTime::currentTime()); \
    relatime = QTime::currentTime();

#endif
