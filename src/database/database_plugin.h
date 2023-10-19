/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _DatabasePlugin_H_
#define _DatabasePlugin_H_

#include <QSqlDatabase>
#include <QObject>
#include <QStringList>
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
    virtual QList<Dbt::Users>                       users(int id) = 0;
    virtual QList<Dbt::Categories>                  categories(const QString& id) = 0;
    virtual QList<Dbt::Categories>                  categoriesToRoot(const QString& id) = 0;
    virtual QList<Dbt::Categories>                  subcategories(const QString& parent) = 0;
    virtual QList<Dbt::UsersCategories>             usersCategories(int id, int user, const QString& category) = 0;
    virtual QList<Dbt::StatusOrder>                 statusOrder(const QString& id) = 0;
    virtual QList<Dbt::Statuses>                    statuses(const QString& id) = 0;
    virtual QList<Dbt::Statuses>                    statuses(const QString& category, const QString& prevstatus) = 0;
    virtual QList<Dbt::Statuses>                    statuses(const QString& category, const QStringList& prevstatuses);

    virtual QList<Dbt::StatusTemplates>             statusTemplates(int id) = 0;

    virtual QList<Dbt::Tickets>                     tickets(bool all) = 0;
    virtual QList<Dbt::Tickets>                     tickets(int ticket, bool all) = 0;
    virtual QList<Dbt::Tickets>                     tickets(const Dbt::Categories&, bool all) = 0;

    virtual QList<Dbt::TicketsVw>                   ticketsVw(bool all) = 0;
    virtual QList<Dbt::TicketsVw>                   ticketsVw(int ticket, bool all) = 0;
    virtual QList<Dbt::TicketsVw>                   ticketsVw(const Dbt::Categories&, bool all) = 0;

    virtual QList<Dbt::TicketStatus>                ticketStatus(int id) = 0;
    virtual QList<Dbt::TicketStatus>                ticketStatus(bool all) = 0;
    virtual QList<Dbt::TicketStatus>                ticketStatus(int ticket, bool all) = 0;

    virtual QList<Dbt::TicketValues>                ticketValues(int id) = 0;
    virtual QList<Dbt::TicketValues>                ticketValues(bool all) = 0;
    virtual QList<Dbt::TicketValues>                ticketValues(int ticket, bool all) = 0;

    virtual QList<Dbt::TicketFiles>                 ticketFiles(int id) = 0;
    virtual QList<Dbt::TicketFiles>                 ticketFiles(bool all) = 0;
    virtual QList<Dbt::TicketFiles>                 ticketFiles(int ticket, bool all) = 0;

    virtual QList<Dbt::TicketTimesheets>            ticketTimesheets(int id) = 0;
    virtual QList<Dbt::TicketTimesheets>            ticketTimesheets(bool all) = 0;
    virtual QList<Dbt::TicketTimesheets>            ticketTimesheets(int ticket, bool all) = 0;
    virtual QList<Dbt::TicketTimesheets>            runningTimesheets(int ticket) = 0;
    virtual QList<Dbt::TicketTimesheets>            startTimesheet(int ticket) = 0;
    virtual QList<Dbt::TicketTimesheets>            stopTimesheet(int ticket) = 0;
    virtual QList<Dbt::TicketTimesheets>            toggleTimesheet(int ticket) = 0;

    virtual QList<Dbt::ClientSettings>              clientSettings() = 0;
    virtual QList<Dbt::Overview>                    overview(const QString& category, const QStringList& statuses) = 0;
    virtual QList<Dbt::Overview>                    overview(const QString& overviewId) = 0;


    virtual void remove(const Dbt::Users& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::UsersCategories& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::Categories& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::StatusOrder& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::Statuses& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::Tickets& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::TicketStatus& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::TicketValues& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::TicketFiles& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::TicketTimesheets& data) { Q_UNUSED(data); }


    virtual QVariant save(const Dbt::Users& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::UsersCategories& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::Categories& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::StatusOrder& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::Statuses& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::Tickets& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::TicketsVw& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::TicketStatus& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::TicketValues& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::TicketFiles& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::TicketTimesheets& data) { Q_UNUSED(data); return QVariant(); }

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
    const AuthenticatedUser *m_authenticatedUser = nullptr;


};

}
}

#endif

