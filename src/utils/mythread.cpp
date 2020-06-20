/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "mythread.h"



MyThread::MyThread(QObject *parent, const QString& name) : QThread(parent) {
    setObjectName(name);
    start();
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}


void MyThread::takeObject(QObject *parent, const QString& name, QObject *x) {
    MyThread *thread = new MyThread(parent, name);
    x->setParent(nullptr);
    x->moveToThread(thread);
    connect(x, SIGNAL(destroyed(QObject *)), thread, SLOT(quit()));
}


