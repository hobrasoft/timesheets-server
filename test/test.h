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
    void getCategories();
    void putCategoriesInsert();
    void putCategoriesUpdate();
    void delCategories();
    void getStatuses();
    void putStatusesInsert();
    void putStatusesInsert2();
    void putStatusesUpdate();
    void getStatusOrder();
    void putStatusOrderInsert();
    void putStatusOrderInsert2();
    void putStatusOrderUpdate2();
    void putStatusOrderUpdate3();
    void delStatuses2();
    void delStatuses();
    //
    void putTickets();                  // insert new testing ticket for following tests, no timesheets, values, files and statuses
    void getTicketsVw1();               // read the ticket inserted
    void putTicketTimesheetsInsert();   // Insert a timesheet to ticket
    void getTicketsVw2();               // Read the ticket with timesheet inserted
    void putTicketTimesheetsUpdate();   // Update the timesheet
    void delTicketTimesheets();         // Delete the timesheet
    //
    void putTicketValuesInsert();       // Insert a timesheet to ticket

private:
    void getTicketsVw3();               // Read the ticket with timesheet inserted
    void putTicketValuesUpdate();       // Update the timesheet
    void delTicketValues();             // Delete the timesheet

  private:
    QSqlDatabase    m_db;
    QProcess       *m_process;
    TestApi::Api   *m_api;

    QString         m_key;
    QString         m_ticket;
    QString         m_ticketTimesheet;
    QString         m_ticketValue;
    int             m_user;
    QString         m_category;
    QDateTime       m_now;
};

#endif

