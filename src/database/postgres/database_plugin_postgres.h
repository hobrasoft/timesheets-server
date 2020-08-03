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

    void setDatabaseName(const QString& x) override { m_databasename = x; }
    void setHostname(const QString& x) override { m_hostname = x; }
    void setPort(int x) override { m_port = x; }
    void setUserName(const QString& x) override { m_username = x; }
    void setPassword(const QString& x) override { m_password = x; }
  
    QList<Dbt::Users>                       authenticate(const QString& login, const QString& password) override;
    QList<Dbt::Categories>                  categories() override;
    QList<Dbt::StatusOrder>                 statusOrder() override;
    QList<Dbt::Statuses>                    statuses() override;
    QList<Dbt::Tickets>                     tickets(bool all) override;
    QList<Dbt::Tickets>                     tickets(int ticket, bool all) override;
    QList<Dbt::TicketsVw>                   ticketsVw(bool all) override;
    QList<Dbt::TicketsVw>                   ticketsVw(int ticket, bool all) override;
    QList<Dbt::TicketStatus>                ticketStatus(int ticket) override;
    QList<Dbt::TicketValues>                ticketValues(int ticket) override;
    QList<Dbt::TicketFiles>                 ticketFiles(int ticket) override;
    QList<Dbt::TicketTimesheets>            ticketTimesheets(int ticket) override;


    void    upgrade();
    bool    open() override; 
    bool    close() override;
    void    begin() override;
    void    commit() override;

  protected:
    void    createTemporaryTableTickets(int ticket, bool all = false);

  private:
    QString         m_databasename;
    QString         m_hostname;
    QString         m_username;
    QString         m_password;
    int             m_port;


};

}
}

#endif

