/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _DatabasePlugin_H_
#define _DatabasePlugin_H_

#include <QSqlDatabase>
#include <QObject>
#include "dbt.h"
#include "database.h"
#include "authenticateduser.h"

namespace Db {
namespace Plugins {

/**
 * @brief
 */
class DatabasePlugin : public QObject {
    Q_OBJECT
  public:
    DatabasePlugin(QObject *);

    void setAuthenticatedUser(const AuthenticatedUser *x) { m_authenticatedUser = x; }
    void setFilter(Database::Filter x) { m_filter = x; }

    virtual void setDatabaseName(const QString&) = 0;
    virtual void setHostname(const QString&) = 0;
    virtual void setPort(int) = 0;
    virtual void setUserName(const QString&) = 0;
    virtual void setPassword(const QString&) = 0;
  
    virtual bool open() = 0;
    virtual bool close() = 0;
    virtual void begin() = 0;
    virtual void commit() = 0;
    virtual QString connectionName() const;

    virtual QList<Dbt::Users>                       authenticate(const QString& user, const QString& password) = 0;
    virtual QList<Dbt::Categories>                  categories() = 0;
    virtual QList<Dbt::StatusOrder>                 statusOrder() = 0;
    virtual QList<Dbt::Statuses>                    statuses() = 0;
    virtual QList<Dbt::Tickets>                     tickets(int ticket) = 0;
    virtual QList<Dbt::TicketsVw>                   ticketsVw(int ticket) = 0;
    virtual QList<Dbt::TicketStatus>                ticketStatus(int ticket) = 0;
    virtual QList<Dbt::TicketValues>                ticketValues(int ticket) = 0;
    virtual QList<Dbt::TicketFiles>                 ticketFiles(int ticket) = 0;
    virtual QList<Dbt::TicketTimesheets>            ticketTimesheets(int ticket) = 0;


  protected:
    int     userId() const;
    bool    userAuthenticated() const;
    Database::Filter    filter() const { return m_filter; }
    QString userLogin() const;
    QString userLang() const;

  protected:
    // virtual QVariant    lastInsertRowId(const QString& sequence_name = QString(), Db::MSqlQuery *q = NULL) = 0;
    // virtual QString     formatLimitOffset(int limit, int offset) const = 0;
    QSqlDatabase m_db;

  public:
    const AuthenticatedUser *m_authenticatedUser;
    Database::Filter    m_filter;


};

}
}

#endif

