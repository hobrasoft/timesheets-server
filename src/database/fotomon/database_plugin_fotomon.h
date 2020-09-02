/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _DatabasePluginFotomon_H_
#define _DatabasePluginFotomon_H_

#include "database_plugin.h"
#include "msqlquery.h"

namespace Db {
namespace Plugins {


/**
 * @brief
 */
class DatabasePluginFotomon : public Db::Plugins::DatabasePlugin {
    Q_OBJECT
  public:
    DatabasePluginFotomon(QObject *);
   ~DatabasePluginFotomon();

    template<typename T> void save(const QList<T>& list) {
        QListIterator<T> iterator(list);
        while (iterator.hasNext()) {
            save(iterator.next());
            }
        }

    void setDatabaseName(const QString& x) Q_DECL_OVERRIDE { m_databasename = x; }
    void setHostname(const QString& x) Q_DECL_OVERRIDE { m_hostname = x; }
    void setPort(int x) Q_DECL_OVERRIDE { m_port = x; }
    void setUserName(const QString& x) Q_DECL_OVERRIDE { m_username = x; }
    void setPassword(const QString& x) Q_DECL_OVERRIDE { m_password = x; }
  
    QList<Dbt::Users>                       authenticate(const QString& login, const QString& password) Q_DECL_OVERRIDE;
    QList<Dbt::Users>                       users(int id) Q_DECL_OVERRIDE;
    QList<Dbt::Categories>                  categories(const QString& id) Q_DECL_OVERRIDE;
    QList<Dbt::Categories>                  subcategories(const QString& id) Q_DECL_OVERRIDE;
    QList<Dbt::UsersCategories>             usersCategories(int id, int user, const QString& category) Q_DECL_OVERRIDE;
    QList<Dbt::StatusOrder>                 statusOrder(const QString& id) Q_DECL_OVERRIDE;
    QList<Dbt::Statuses>                    statuses(const QString& id) Q_DECL_OVERRIDE;
    QList<Dbt::Tickets>                     tickets(bool all) Q_DECL_OVERRIDE;
    QList<Dbt::Tickets>                     tickets(int ticket, bool all) Q_DECL_OVERRIDE;
    QList<Dbt::Tickets>                     tickets(const Dbt::Categories&) Q_DECL_OVERRIDE;
    QList<Dbt::TicketsVw>                   ticketsVw(bool all) Q_DECL_OVERRIDE;
    QList<Dbt::TicketsVw>                   ticketsVw(int ticket, bool all) Q_DECL_OVERRIDE;
    QList<Dbt::TicketsVw>                   ticketsVw(const Dbt::Categories&) Q_DECL_OVERRIDE;

    QList<Dbt::TicketStatus>                ticketStatus(int id) Q_DECL_OVERRIDE;
    QList<Dbt::TicketStatus>                ticketStatus(int ticket, bool all) Q_DECL_OVERRIDE;
    QList<Dbt::TicketStatus>                ticketStatus(bool all) Q_DECL_OVERRIDE;

    QList<Dbt::TicketValues>                ticketValues(int id) Q_DECL_OVERRIDE;
    QList<Dbt::TicketValues>                ticketValues(int ticket, bool all) Q_DECL_OVERRIDE;
    QList<Dbt::TicketValues>                ticketValues(bool all) Q_DECL_OVERRIDE;

    QList<Dbt::TicketFiles>                 ticketFiles(int id) Q_DECL_OVERRIDE;
    QList<Dbt::TicketFiles>                 ticketFiles(int ticket, bool all) Q_DECL_OVERRIDE;
    QList<Dbt::TicketFiles>                 ticketFiles(bool all) Q_DECL_OVERRIDE;

    QList<Dbt::TicketTimesheets>            ticketTimesheets(int id) Q_DECL_OVERRIDE;
    QList<Dbt::TicketTimesheets>            ticketTimesheets(int ticket, bool all) Q_DECL_OVERRIDE;
    QList<Dbt::TicketTimesheets>            ticketTimesheets(bool all) Q_DECL_OVERRIDE;

    QList<Dbt::ClientSettings>              clientSettings() Q_DECL_OVERRIDE;

    QVariant save(const Dbt::Tickets& data) Q_DECL_OVERRIDE;
    QVariant save(const Dbt::TicketsVw& data) Q_DECL_OVERRIDE;
    QVariant save(const Dbt::TicketStatus& data) Q_DECL_OVERRIDE;
    QVariant save(const Dbt::TicketValues& data) Q_DECL_OVERRIDE;
    QVariant save(const Dbt::TicketFiles& data) Q_DECL_OVERRIDE;
    QVariant save(const Dbt::TicketTimesheets& data) Q_DECL_OVERRIDE;

    void remove(const Dbt::Tickets& data) Q_DECL_OVERRIDE;
    void remove(const Dbt::TicketValues& data) Q_DECL_OVERRIDE;
    void remove(const Dbt::TicketFiles& data) Q_DECL_OVERRIDE;
    void remove(const Dbt::TicketTimesheets& data) Q_DECL_OVERRIDE;

    bool    open() Q_DECL_OVERRIDE; 
    bool    close() Q_DECL_OVERRIDE;
    void    begin() Q_DECL_OVERRIDE;
    void    commit() Q_DECL_OVERRIDE;

  private:
    void            createTemporaryTableCategories();
    void            createTemporaryTableTickets(int ticket, bool all);
    static QString  parentCategoryKey(const QVariant& type, const QVariant& system, const QVariant& category, int parent_type);
    static QString        categoryKey(const QVariant& type, const QVariant& system, const QVariant& category, int parent_type);
    QVariant        currval(const QString& sequence);


    bool            m_temporaryTableTicketsCreated;
    QString         m_databasename;
    QString         m_hostname;
    QString         m_username;
    QString         m_password;
    int             m_port;


};

}
}

#endif

