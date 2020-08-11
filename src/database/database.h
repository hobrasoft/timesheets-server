/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _Database_H_
#define _Database_H_

#include <QObject>
#include <QVariant>
#include "authenticateduser.h"
#include "dbt.h"
#include "database_plugin.h"


namespace Db {

struct PluginDescription {
    QString     name;
    QString     description;
    PluginDescription(const QString& n, const QString& d) { name=n; description=d; }
};

/**
 * @brief
 */
class Database : public QObject {
    Q_OBJECT
    Db::Plugins::DatabasePlugin *m_dbplugin;

  public:

   ~Database();
    static Database *instance(QObject *parent = nullptr);
    static Database *create(QObject *parent);
    static Database *create(QObject *parent, const AuthenticatedUser *u);
    bool isOpen() const { return m_isOpen; }
    QString     connectionName() const;

    QList<Dbt::Users>                       authenticate(const QString& user, const QString& password);
    QList<Dbt::Users>                       users(int id = -1);
    QList<Dbt::Categories>                  categories(const QString& id = QString());
    QList<Dbt::StatusOrder>                 statusOrder(const QString& id = QString());
    QList<Dbt::Statuses>                    statuses(const QString& id = QString());

    QList<Dbt::Tickets>                     tickets(bool all = false);
    QList<Dbt::Tickets>                     tickets(int ticket = -1, bool all = false);

    QList<Dbt::TicketsVw>                   ticketsVw(bool all = false);
    QList<Dbt::TicketsVw>                   ticketsVw(int ticket = -1, bool all = false);

    QList<Dbt::TicketStatus>                ticketStatus(int id);
    QList<Dbt::TicketStatus>                ticketStatus(bool all = false);
    QList<Dbt::TicketStatus>                ticketStatus(int ticket = -1, bool all = false);

    QList<Dbt::TicketValues>                ticketValues(int id = -1);
    QList<Dbt::TicketValues>                ticketValues(bool all = false);
    QList<Dbt::TicketValues>                ticketValues(int ticket, bool all);

    QList<Dbt::TicketFiles>                 ticketFiles(int id = -1);
    QList<Dbt::TicketFiles>                 ticketFiles(bool all = false);
    QList<Dbt::TicketFiles>                 ticketFiles(int ticket = -1, bool all = false);

    QList<Dbt::TicketTimesheets>            ticketTimesheets(int id);
    QList<Dbt::TicketTimesheets>            ticketTimesheets(bool all);
    QList<Dbt::TicketTimesheets>            ticketTimesheets(int ticket, bool all);

    template<typename T> QVariant save(const T& id) { return m_dbplugin->save(id); }
    template<typename T> void remove(const T& id) { m_dbplugin->remove(id); }

    void        begin();
    void        commit();

  signals:
    void    opened(bool);

  public slots:
    bool    open();
    void    close();

  private slots:

  private:
    static Database *m_instance;
    Database(QObject *parent);
    bool m_isOpen;

};

}

#endif

