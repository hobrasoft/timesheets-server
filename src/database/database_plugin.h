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
    virtual QList<Dbt::Tickets>                     tickets() = 0;
    virtual QList<Dbt::TicketStatus>                ticketStatus() = 0;


  protected:
    int     userId() const;
    bool    userAuthenticated() const;
    QString userLogin() const;
    QString userLang() const;

  protected:
    // virtual QVariant    lastInsertRowId(const QString& sequence_name = QString(), Db::MSqlQuery *q = NULL) = 0;
    // virtual QString     formatLimitOffset(int limit, int offset) const = 0;
    QSqlDatabase m_db;

  public:
    const AuthenticatedUser *m_authenticatedUser;


};

}
}

#endif

