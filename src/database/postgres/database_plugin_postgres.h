/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _DatabasePluginPostgres_H_
#define _DatabasePluginPostgres_H_

#include "database_plugin.h"
#include "msqlquery.h"

namespace Db {
namespace Plugins {


/**
 * @brief
 */
class DatabasePluginPostgres : public Db::Plugins::DatabasePlugin {
    Q_OBJECT
  public:
    DatabasePluginPostgres(QObject *);
   ~DatabasePluginPostgres();

    void setDatabaseName(const QString& x) Q_DECL_OVERRIDE { m_databasename = x; }
    void setHostname(const QString& x) Q_DECL_OVERRIDE { m_hostname = x; }
    void setPort(int x) Q_DECL_OVERRIDE { m_port = x; }
    void setUserName(const QString& x) Q_DECL_OVERRIDE { m_username = x; }
    void setPassword(const QString& x) Q_DECL_OVERRIDE { m_password = x; }

    void                                    changePassword(const QString& login, const QString& oldpassword, const QString& newpassword) Q_DECL_OVERRIDE;
    QList<Dbt::Users>                       authenticate(const QString& login, const QString& password) Q_DECL_OVERRIDE;
    QList<Dbt::Users>                       users(int id) Q_DECL_OVERRIDE;
    QList<Dbt::UsersCategories>             usersCategories(int id, int user, const QString& category) Q_DECL_OVERRIDE;
    QList<Dbt::Categories>                  categories(const QString& id) Q_DECL_OVERRIDE;
    QList<Dbt::Categories>                  categoriesToRoot(const QString& id) Q_DECL_OVERRIDE;
    QList<Dbt::Categories>                  subcategories(const QString& id) Q_DECL_OVERRIDE;
    QList<Dbt::Categories>                  siblingcategories(const QString& id) Q_DECL_OVERRIDE;
    QList<Dbt::StatusOrder>                 statusOrder(const QString& id) Q_DECL_OVERRIDE;
    QList<Dbt::Statuses>                    statuses(const QString&) Q_DECL_OVERRIDE;
    QList<Dbt::Statuses>                    statuses(const QString&, const QString&) Q_DECL_OVERRIDE;

    QList<Dbt::StatusTemplates>             statusTemplates(int id) Q_DECL_OVERRIDE;
    QList<Dbt::Tickets>                     tickets(bool all) Q_DECL_OVERRIDE;
    QList<Dbt::Tickets>                     tickets(int ticket, bool all) Q_DECL_OVERRIDE;
    QList<Dbt::Tickets>                     tickets(const Dbt::Categories&, bool all) Q_DECL_OVERRIDE;
    QList<Dbt::TicketsVw>                   ticketsVw(bool all) Q_DECL_OVERRIDE;
    QList<Dbt::TicketsVw>                   ticketsVw(int ticket, bool all) Q_DECL_OVERRIDE;
    QList<Dbt::TicketsVw>                   ticketsVw(const Dbt::Categories&, bool all) Q_DECL_OVERRIDE;

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
    QList<Dbt::TicketTimesheets>            runningTimesheets(int ticket) Q_DECL_OVERRIDE;
    QList<Dbt::TicketTimesheets>            startTimesheet(int ticket) Q_DECL_OVERRIDE;
    QList<Dbt::TicketTimesheets>            stopTimesheet(int ticket) Q_DECL_OVERRIDE;
    QList<Dbt::TicketTimesheets>            toggleTimesheet(int ticket) Q_DECL_OVERRIDE;

    QList<Dbt::ClientSettings>              clientSettings() Q_DECL_OVERRIDE;
    QList<Dbt::Overview>                    overview(const QString& category, const QStringList& statuses) Q_DECL_OVERRIDE;
    QList<Dbt::Overview>                    overview(const QString& overviewId) Q_DECL_OVERRIDE;
    QList<Dbt::OverviewList>                overviewList() Q_DECL_OVERRIDE;
    QList<Dbt::ServerInfo>                  serverInfo() Q_DECL_OVERRIDE;

    template<typename T> void save(const QList<T>& list) {
        QListIterator<T> iterator(list);
        while (iterator.hasNext()) {
            save(iterator.next());
            }
        }


    void remove(const Dbt::Users& data) Q_DECL_OVERRIDE;
    void remove(const Dbt::Categories& data) Q_DECL_OVERRIDE;
    void remove(const Dbt::StatusOrder& data) Q_DECL_OVERRIDE;
    void remove(const Dbt::Statuses& data) Q_DECL_OVERRIDE;
    void remove(const Dbt::Tickets& data) Q_DECL_OVERRIDE;
    void remove(const Dbt::TicketStatus& data) Q_DECL_OVERRIDE;
    void remove(const Dbt::TicketValues& data) Q_DECL_OVERRIDE;
    void remove(const Dbt::TicketFiles& data) Q_DECL_OVERRIDE;
    void remove(const Dbt::TicketTimesheets& data) Q_DECL_OVERRIDE;
    void remove(const Dbt::OverviewList& data) Q_DECL_OVERRIDE;


    QVariant save(const Dbt::Users& data) Q_DECL_OVERRIDE;
    QVariant save(const Dbt::Categories& data) Q_DECL_OVERRIDE;
    QVariant save(const Dbt::StatusOrder& data) Q_DECL_OVERRIDE;
    QVariant save(const Dbt::Statuses& data) Q_DECL_OVERRIDE;
    QVariant save(const Dbt::Tickets& data) Q_DECL_OVERRIDE;
    QVariant save(const Dbt::TicketsVw& data) Q_DECL_OVERRIDE;
    QVariant save(const Dbt::TicketStatus& data) Q_DECL_OVERRIDE;
    QVariant save(const Dbt::TicketValues& data) Q_DECL_OVERRIDE;
    QVariant save(const Dbt::TicketFiles& data) Q_DECL_OVERRIDE;
    QVariant save(const Dbt::TicketTimesheets& data) Q_DECL_OVERRIDE;
    QVariant save(const Dbt::ServerInfo& data) Q_DECL_OVERRIDE;


    void    upgrade();
    bool    open() Q_DECL_OVERRIDE; 
    bool    close() Q_DECL_OVERRIDE;
    void    begin() Q_DECL_OVERRIDE;
    void    commit() Q_DECL_OVERRIDE;

  protected:
    void    createTemporaryTableTickets (int ticket, bool all = false);
    QVariant currval(const QString& sequence);
    void     removeTicketsDetails(int ticket);

  private:
    QString         m_databasename;
    QString         m_hostname;
    QString         m_username;
    QString         m_password;
    int             m_port;

    static bool     m_upgraded;
    bool            m_temporaryTableTicketsCreated;

};

}
}

#endif

