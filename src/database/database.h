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


namespace Db {

struct PluginDescription {
    QString     name;
    QString     description;
    PluginDescription(const QString& n, const QString& d) { name=n; description=d; }
};

namespace Plugins {
class DatabasePlugin;
}

/**
 * @brief
 */
class Database : public QObject {
    Q_OBJECT
  public:

   ~Database();
    static Database *instance(QObject *parent = nullptr);
    static Database *create(QObject *parent);
    static Database *create(QObject *parent, const AuthenticatedUser *u);
    bool isOpen() const { return m_isOpen; }
    QString     connectionName() const;

    QList<Dbt::Users>                       authenticate(const QString& user, const QString& password);
    QList<Dbt::Categories>                  categories(const QString& id = QString());
    QList<Dbt::StatusOrder>                 statusOrder(const QString& id = QString());
    QList<Dbt::Statuses>                    statuses(const QString& id = QString());
    QList<Dbt::Tickets>                     tickets(bool all = false);
    QList<Dbt::Tickets>                     tickets(int ticket = -1, bool all = false);
    QList<Dbt::TicketsVw>                   ticketsVw(bool all = false);
    QList<Dbt::TicketsVw>                   ticketsVw(int ticket = -1, bool all = false);
    QList<Dbt::TicketStatus>                ticketStatus(int ticket = -1);
    QList<Dbt::TicketValues>                ticketValues(int ticket = -1);
    QList<Dbt::TicketFiles>                 ticketFiles(int ticket = -1);
    QList<Dbt::TicketTimesheets>            ticketTimesheets(int ticket = -1);

    void        remove(const Dbt::Categories&);
    void        save(const Dbt::Categories&);

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

    Db::Plugins::DatabasePlugin *m_dbplugin;

};

}

#endif

