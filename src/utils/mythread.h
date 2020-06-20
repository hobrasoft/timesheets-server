/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _MyThread_H_
#define _MyThread_H_

#include <QThread>
#include <QObject>

/**
 * @brief
 */
class MyThread : QThread {
    Q_OBJECT
  public:
    static void takeObject(QObject *parent, const QString& name, QObject *);
  private:
    MyThread(QObject *parent, const QString& name);
};

#endif

