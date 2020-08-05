/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _Test_H_
#define _Test_H_

#include <QObject>
#include <QTest>
#include <QSqlDatabase>
#include <QProcess>
#include "api.h"


/**
 * @brief
 */
class Test : public QObject {
    Q_OBJECT
  public:
    Test();
    TestApi::Api *api() { return m_api; }

  private slots:
    void initTestCase();
    void cleanupTestCase();

    void getServerInfo();
    void authenticate();
    void getUsers();
    void putUsers();
    void delUsers();

  private:
    QSqlDatabase  m_db;
    QProcess     *m_process;
    TestApi::Api *m_api;

    QString       m_key;
};

#endif

