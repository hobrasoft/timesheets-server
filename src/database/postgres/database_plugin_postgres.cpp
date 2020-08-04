/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "database_plugin_postgres.h"
#include "msettings.h"
#include "msqlquery.h"
#include "json.h"
#include "pdebug.h"
#include <QUuid>
#include <QSqlError>
#include <QCryptographicHash>
#include <QFile>

using namespace Db::Plugins;


DatabasePluginPostgres::~DatabasePluginPostgres() {
    close();
}


DatabasePluginPostgres::DatabasePluginPostgres(QObject *parent) : Db::Plugins::DatabasePlugin(parent) {
    Q_ASSERT(parent != NULL);
    setObjectName("DatabasePluginPostgres");
}


bool DatabasePluginPostgres::open() {
    m_db = QSqlDatabase::addDatabase("QPSQL", QUuid::createUuid().toString().toUtf8());

    m_db.setDatabaseName ( m_databasename );
    m_db.setHostName     ( m_hostname     );
    m_db.setPort         ( m_port         );
    m_db.setUserName     ( m_username     );
    m_db.setPassword     ( m_password     );

    if (!m_db.open()) {
        PDEBUG << "Cannot connect to database Postgres";
        PDEBUG << m_db.lastError().text();
        return false;
        }

    upgrade();

    return true;
}


void DatabasePluginPostgres::upgrade() {
    PDEBUG;
    MSqlQuery q(m_db);
    q.exec("select version from version;");
    int version = (q.next()) ? q.value(0).toInt() : -1;
    for (;;) {
        version++;
        QString patchname = QString(":/postgres/patch.%1.sql").arg(version, 3, 10, QChar('0'));
        QFile file(patchname);
        if (!file.open(QIODevice::ReadOnly)) {
            return;
            }
        PDEBUG << "aplying db patch " << patchname;

        while (!file.atEnd()) {
            QByteArray command;
            QByteArray line;
            do  {
                line = file.readLine();
                command += line;
                } while (!line.contains(";") && !file.atEnd());
            command = command.trimmed();
            if (command.isEmpty()) { continue; }
            MSqlQuery q(m_db);
            if (!q.exec(QString::fromUtf8(command))) {
                QSqlError e = q.lastError();
                if (e.type() != QSqlError::NoError) {
                    PDEBUG << q.lastQuery();
                    PDEBUG << e.databaseText();
                    PDEBUG << e.driverText();
                    return;
                    }
                }
            }

        q.exec(QString("update version set version = %1;").arg(version));
        }

    return;
}


bool DatabasePluginPostgres::close() {
    m_db.close();
    return true;
}


void DatabasePluginPostgres::begin() {
    MSqlQuery q(m_db);
    q.exec("begin;");
}


void DatabasePluginPostgres::commit() {
    MSqlQuery q(m_db);
    q.exec("commit;");
}

  
QList<Dbt::Users> DatabasePluginPostgres::authenticate(const QString& login, const QString& password) {
    QString md5 = QString::fromUtf8(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex());
    QList<Dbt::Users> list;
    MSqlQuery q(m_db);
    q.prepare("select \"user\", login, name, lang from users where login = :login and password = :password and enabled;");
    q.bindValue(":login", login);
    q.bindValue(":password", md5);
    q.exec();
    while (q.next()) {
        int i=0;
        Dbt::Users x;
        x.user          = q.value(i++).toInt();
        x.login         = q.value(i++).toString();
        x.name          = q.value(i++).toString();
        x.lang          = q.value(i++).toString();
        list << x;
        }
    return list;
}


QList<Dbt::Categories> DatabasePluginPostgres::categories(const QString& id) {
    QList<Dbt::Categories> list;
    MSqlQuery q(m_db);

    q.prepare(R"'(
        select c.category, c.parent_category, c.description, c.price 
        from categories c, users_categories uc
        where c.category = uc.category
          and uc."user" = :user
          and (:id1 <= 0 or :id2 = c.category);
        )'");
    q.bindValue(":user", userId());
    q.bindValue(":id1", id.toInt());
    q.bindValue(":id2", id.toInt());
    PDEBUG << q.lastBoundQuery();
    q.exec();
    while (q.next()) {
        int i=0;
        Dbt::Categories x;
        x.category = q.value(i++).toString();
        x.parent_category = q.value(i++).toString();
        x.description = q.value(i++).toString();
        x.price = q.value(i++).toDouble();
        list << x;
        }

    PDEBUG << "pocet kategorii" << list.size();
    return list;
}


void DatabasePluginPostgres::remove(const Dbt::Categories& id) {
    QList<Dbt::StatusOrder> list;
    MSqlQuery q(m_db);
    q.prepare(R"'(delete from categories where category = :id;)'");
    q.bindValue(":id", id.category);
    q.exec();
}


void DatabasePluginPostgres::save(const Dbt::Categories& data) {
    QList<Dbt::StatusOrder> list;
    MSqlQuery q(m_db);

    q.prepare(R"'(
        update categories set
            parent_category = :parent_category,
            description = :description,
            price = :price,
            where category = :category1
        )'");
    q.bindValue(":category1", data.category);
    q.bindValue(":parent_category", data.parent_category);
    q.bindValue(":description", data.description);
    q.bindValue(":price", data.price);
    q.exec();

    q.prepare(R"'(
        insert into categories (category, parent_category, description, price)
            select :category1, :parent_category, :description, :price
            where not exists (select 1 from categories where category = :category2);
        )'");
    q.bindValue(":category1", data.category);
    q.bindValue(":category2", data.category);
    q.bindValue(":parent_category", data.parent_category);
    q.bindValue(":description", data.description);
    q.bindValue(":price", data.price);
    q.exec();

}


QList<Dbt::StatusOrder> DatabasePluginPostgres::statusOrder(const QString& id) {
    QList<Dbt::StatusOrder> list;
    MSqlQuery q(m_db);

    q.exec(R"'(
        select category, previous_status, next_status from status_order
        where (:id1 <= 0 or :id2 = id);
        )'");
    q.bindValue(":id1", id.toInt());
    q.bindValue(":id2", id.toInt());
    while (q.next()) {
        int i = 0;
        Dbt::StatusOrder x;
        PDEBUG << q.value(0).isValid() << q.value(0).isNull();
        x.category          = q.value(i++);
        x.previous_status   = q.value(i++);
        x.next_status       = q.value(i++);
        PDEBUG << x.toMap();
        list << x;
        }

    return list;

}


QList<Dbt::Statuses> DatabasePluginPostgres::statuses(const QString& id) {
    QList<Dbt::Statuses> list;
    MSqlQuery q(m_db);

    q.prepare(R"'(
        select status, description, abbreviation, color from statuses
        where (:id1 = '' or :id2 = status)
        )'");
    q.exec();
    q.bindValue(":id1", id);
    q.bindValue(":id2", id);
    while (q.next()) {
        int i=0;
        Dbt::Statuses x;
        x.status = q.value(i++).toString();
        x.description = q.value(i++).toString();
        x.abbreviation = q.value(i++).toString();
        x.color = q.value(i++).toString();
        list << x;
        }

    return list;
}


void DatabasePluginPostgres::createTemporaryTableTickets(int ticket, bool all) {
    MSqlQuery q(m_db);
    q.exec(R"'(
        create temporary table temporary_tickets (
            ticket      int,
            category    int,
            date        timestamp with time zone,
            price       numeric,
            description text,
            "user"      int
            );
        )'");

    if (all) {
        PDEBUG << "Vybiram VSE";
        q.prepare(R"'(
            insert into temporary_tickets (ticket, category, date, price, description, "user")
            select t.ticket, t.category, t.date, t.price, t.description, t."user"
                from tickets t, users_categories uc
                where t.category = uc.category
                  and uc."user" = :user
                  and (:ticket1 <= 0 or :ticket2 = t.ticket)
            ;
            )'");
      } else {
        PDEBUG << "Vybiram pouze otevrene";
        q.prepare(R"'(
            with
            ending_status as (
                select status from statuses where closed
                ),
            tickets_last_status as (
                select t.ticket, tl.status
                    from tickets t
                    left join lateral (select tn.ticket, tn.status from ticket_status tn where tn.ticket = t.ticket order by ticket, date desc limit 1) tl using (ticket)
                ),
            closed_tickets as (
                select distinct ts.ticket from tickets_last_status ts, ending_status es where ts.status = es.status
                ),
            active_tickets as (
                select t1.ticket from tickets t1 where t1.ticket not in (select ticket from closed_tickets)
                )

            insert into temporary_tickets (ticket, category, date, price, description, "user")
            select t.ticket, t.category, t.date, t.price, t.description, t."user"
                from tickets t, users_categories uc
                where t.ticket in (select ticket from active_tickets)
                  and uc."user" = :user
                  and t.category = uc.category
                  and (:ticket1 <= 0 or :ticket2 = t.ticket)
            ;
            )'");
        }
    q.bindValue(":user", userId());
    q.bindValue(":ticket1", ticket);
    q.bindValue(":ticket2", ticket);
    q.exec();

}


QList<Dbt::Tickets> DatabasePluginPostgres::tickets(bool all) {
    return tickets(-1, all);
}


QList<Dbt::Tickets> DatabasePluginPostgres::tickets(int ticket, bool all) {
    PDEBUG;
    createTemporaryTableTickets(ticket, all);
    QList<Dbt::Tickets> list;
    MSqlQuery q(m_db);

    q.prepare(R"'(
        select ticket, category, date, price, description, "user"
            from temporary_tickets ;
        )'");
    q.exec();
    while (q.next()) {
        Dbt::Tickets x;
        int i=0;
        x.ticket        = q.value(i++);
        x.category      = q.value(i++);
        x.date          = q.value(i++).toDateTime();
        x.price         = q.value(i++);
        x.description   = q.value(i++).toString();
        x.user          = q.value(i++).toInt();
        list << x;
        }
    return list;
}


QList<Dbt::TicketStatus> DatabasePluginPostgres::ticketStatus(int ticket) {
    PDEBUG;
    createTemporaryTableTickets(ticket);
    QList<Dbt::TicketStatus> list;
    MSqlQuery q(m_db);

    q.prepare(R"'(
        select ts.id, ts.ticket, ts."user", u.name, ts.date, ts.description, ts.status
            from temporary_tickets t, ticket_status ts, users u
            where t.ticket = ts.ticket
              and u."user" = ts."user"
        )'");
    q.exec();
    while (q.next()) {
        Dbt::TicketStatus x;
        int i=0;
        x.id            = q.value(i++);
        x.ticket        = q.value(i++);
        x.user          = q.value(i++);
        x.user_name     = q.value(i++).toString();
        x.date          = q.value(i++).toDateTime();
        x.description   = q.value(i++).toString();
        x.status        = q.value(i++);
        list << x;
        }
    return list;
}


QList<Dbt::TicketsVw> DatabasePluginPostgres::ticketsVw(bool all) {
    return ticketsVw(-1, all);
}


QList<Dbt::TicketsVw> DatabasePluginPostgres::ticketsVw(int ticket, bool all) {
    createTemporaryTableTickets(ticket);
    QList<Dbt::TicketsVw> list;
    return list;
}


QList<Dbt::TicketTimesheets> DatabasePluginPostgres::ticketTimesheets(int ticket) {
    createTemporaryTableTickets(ticket);
    QList<Dbt::TicketTimesheets> list;
    MSqlQuery q(m_db);
    q.prepare(R"'(
        select tt.id, tt.ticket, tt."user", tt.date, tt.description, tt.status
            from temporary_tickets t, tickets_timesheets tt
            where t.ticket = tt.ticket
              and :user = tt."user"
            ;
        )'");
    q.bindValue(":user", userId());
    q.exec();
    while (q.next()) {
        int i=0;
        Dbt::TicketTimesheets x;
        x.id            = q.value(i++);
        x.ticket        = q.value(i++);
        x.date_from     = q.value(i++).toDateTime();
        x.date_to       = q.value(i++).toDateTime();
        list << x;
        }
    return list;
}


QList<Dbt::TicketFiles> DatabasePluginPostgres::ticketFiles(int ticket) {
    QList<Dbt::TicketFiles> list;
    return list;
}


QList<Dbt::TicketValues> DatabasePluginPostgres::ticketValues(int ticket) {
    createTemporaryTableTickets(ticket);
    QList<Dbt::TicketValues> list;
    MSqlQuery q(m_db);
    q.prepare(R"'(
        select tv.id, tt.ticket, tt.name, tt.value
            from temporary_tickets t, tickets_values tv
            where t.ticket = tv.ticket
            ;
        )'");
    q.exec();
    while (q.next()) {
        int i=0;
        Dbt::TicketValues x;
        x.id            = q.value(i++);
        x.ticket        = q.value(i++);
        x.name          = q.value(i++).toString();
        x.value         = q.value(i++).toString();
        list << x;
        }
    return list;
}


