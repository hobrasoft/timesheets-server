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

namespace Db::Plugins {
    QString null(const QString& x) {
        return (x == "0") ? "" : x;
        }
}

bool DatabasePluginPostgres::m_upgraded = false;


DatabasePluginPostgres::~DatabasePluginPostgres() {
    close();
}


DatabasePluginPostgres::DatabasePluginPostgres(QObject *parent) : Db::Plugins::DatabasePlugin(parent) {
    Q_ASSERT(parent != NULL);
    setObjectName("DatabasePluginPostgres");
    m_temporaryTableTicketsCreated = false;
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
    if (m_upgraded) { return; }
    PDEBUG;
    m_upgraded = true;
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
    q.prepare("select \"user\", login, name, lang, enabled, admin from users where login = :login and password = :password and enabled;");
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
        x.enabled       = q.value(i++).toBool();
        x.admin         = q.value(i++).toBool();
        list << x;
        }
    return list;
}


QList<Dbt::Users> DatabasePluginPostgres::users(int id) {
    QList<Dbt::Users> list;
    MSqlQuery q(m_db);
    q.prepare(R"'(
        select "user", login, name, lang, enabled, admin 
            from users 
            where (:id1 <= 0 or :id2 = "user");
        )'");
    q.bindValue(":id1", id);
    q.bindValue(":id2", id);
    q.exec();
    while (q.next()) {
        int i=0;
        Dbt::Users x;
        x.user          = q.value(i++).toInt();
        x.login         = q.value(i++).toString();
        x.name          = q.value(i++).toString();
        x.lang          = q.value(i++).toString();
        x.enabled       = q.value(i++).toBool();
        x.admin         = q.value(i++).toBool();
        list << x;
        }

    return list;
}


void DatabasePluginPostgres::remove(const Dbt::Categories& id) {
    MSqlQuery q(m_db);
    q.prepare(R"'(delete from categories where category = :id;)'");
    q.bindValue(":id", id.category);
    q.exec();
}


QVariant DatabasePluginPostgres::currval(const QString& sequence) {
    MSqlQuery q(m_db);
    QVariant cv;
    q.prepare(R"'(select currval(:sequence);)'");
    q.bindValue(":sequence", sequence);
    q.exec();
    if (q.next()) {
        cv = q.value(0);
        }
    return cv;
}


QVariant DatabasePluginPostgres::save(const Dbt::Categories& data) {
    MSqlQuery q(m_db);

    QVariant parent_category = (data.parent_category.toInt() > 0) 
                             ?  data.parent_category.toInt()
                             : QVariant();

    QVariant        category = (data.category.toInt() > 0) 
                             ?  data.category.toInt()
                             : QVariant(QVariant::Int);

    // TODO: Kontrola, aby nešlo založit novou kategorii v nadřízené kategorii bez přístupu

    q.prepare(R"'(select 1 from categories where category = :category;)'");
    q.bindValue(":category", data.category);
    q.exec();
    if (q.next()) {
        q.prepare(R"'(
            update categories set
                parent_category = ?,
                description = ?,
                price = ?
                where category = ?
            )'");
        q.bindValue(0, parent_category);
        q.bindValue(1, data.description);
        q.bindValue(2, data.price);
        q.bindValue(3, category);
        q.exec();
        return QVariant(data.category);
        }

    q.prepare(R"string(
        insert into categories (parent_category, description, price) values (?, ?, ?);
        )string");
    q.bindValue(0, parent_category);
    q.bindValue(1, data.description);
    q.bindValue(2, data.price);
    q.exec();

    q.prepare(R"string(
        insert into users_categories ("user", category) values (?, currval('categories_category_seq'));
        )string");
    q.bindValue(0, userId());
    q.exec();

    return currval("categories_category_seq");

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
    q.exec();
    while (q.next()) {
        int i=0;
        Dbt::Categories x;
        x.category = q.value(i++).toString();
        x.parent_category = null(q.value(i++).toString());
        x.description = q.value(i++).toString();
        x.price = q.value(i++).toDouble();
        list << x;
        }

    return list;
}


void DatabasePluginPostgres::remove(const Dbt::Users& id) {
    MSqlQuery q(m_db);

    q.prepare(R"'(select 1 from users where "user" = :id and admin and enabled;)'");
    q.bindValue(":id", userId());
    q.exec();
    if (!q.next()) { return; }

    q.prepare(R"'(delete from users where "user" = :id;)'");
    q.bindValue(":id", id.user);
    q.exec();
}


QVariant DatabasePluginPostgres::save(const Dbt::Users& data) {
    MSqlQuery q(m_db);

    q.prepare(R"'(select 1 from users where "user" = :id and admin and enabled;)'");
    q.bindValue(":id", userId());
    q.exec();
    if (!q.next()) { return QVariant(); }


    q.prepare(R"'(select 1 from users where "user" = :id;)'");
    q.bindValue(":id", data.user);
    q.exec();
    if (q.next()) { 
        q.prepare(R"'(
            update users set
                login = :login,
                name = :name,
                lang = :lang,
                enabled = :enabled,
                admin = :admin
                where "user" = :id1
            )'");
        q.bindValue(":id1", data.user);
        q.bindValue(":login", data.login);
        q.bindValue(":name", data.name);
        q.bindValue(":lang", data.lang);
        q.bindValue(":enabled", data.enabled);
        q.bindValue(":admin", data.admin);
        q.exec();
        return QVariant(data.user);
      } else {

        q.prepare(R"'(
            insert into users (login, name, lang, enabled, admin)
                select :login, :name, :lang, :enabled, :admin
                where not exists (select 1 from users where "user" = :id1);
            )'");
        q.bindValue(":id1", data.user);
        q.bindValue(":login", data.login);
        q.bindValue(":name", data.name);
        q.bindValue(":lang", data.lang);
        q.bindValue(":enabled", data.enabled);
        q.bindValue(":admin", data.admin);
        q.exec();

        return currval("users_user_seq");
        }

    return QVariant();

}


QList<Dbt::StatusOrder> DatabasePluginPostgres::statusOrder(const QString& id) {
    QList<Dbt::StatusOrder> list;
    MSqlQuery q(m_db);

    q.prepare(R"'(
        select id, category, previous_status, next_status from status_order
        where (:id1 <= 0 or :id2 = id);
        )'");
    q.bindValue(":id1", id.toInt());
    q.bindValue(":id2", id.toInt());
    q.exec();
    while (q.next()) {
        int i = 0;
        Dbt::StatusOrder x;
        x.id                = q.value(i++).toInt();
        x.category          = q.value(i++);
        x.previous_status   = q.value(i++);
        x.next_status       = q.value(i++);
        list << x;
        }

    return list;

}


void DatabasePluginPostgres::remove(const Dbt::StatusOrder& id) {
    MSqlQuery q(m_db);
    q.prepare(R"'(delete from status_order where id = :id;)'");
    q.bindValue(":id", id.id);
    q.exec();
}


QVariant DatabasePluginPostgres::save(const Dbt::StatusOrder& data) {
    MSqlQuery q(m_db);

    q.prepare(R"'(select 1 from status_order where id = :id;)'");
    q.bindValue(":id", data.id);
    q.exec();
    if (q.next()) {
        q.prepare(R"'(
            update status_order set
                category = :category,
                previous_status = :previous_status,
                next_status = :next_status
                where id = :id
            )'");
        q.bindValue(":category", data.category);
        q.bindValue(":previous_status", data.previous_status);
        q.bindValue(":next_status", data.next_status);
        q.bindValue(":id", data.id);
        q.exec();
        return QVariant(data.id);

      } else {

        q.prepare(R"'(
            insert into status_order (category, previous_status, next_status)
                select :category, :previous_status, :next_status
            )'");
        q.bindValue(":category", data.category);
        q.bindValue(":previous_status", data.previous_status);
        q.bindValue(":next_status", data.next_status);
        q.bindValue(":id", data.id);
        q.exec();
        return currval("status_order_id_seq");
        }

    return QVariant();

}


void DatabasePluginPostgres::createTemporaryTableTickets(int ticket, bool all) {
    if (m_temporaryTableTicketsCreated) { return; }
    m_temporaryTableTicketsCreated = true;
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
            ticket_last_status as (
                select t.ticket, tl.status
                    from tickets t
                    left join lateral (select tn.ticket, tn.status from ticket_status tn where tn.ticket = t.ticket order by ticket, date desc limit 1) tl using (ticket)
                ),
            closed_tickets as (
                select distinct ts.ticket from ticket_last_status ts, ending_status es where ts.status = es.status
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


QList<Dbt::TicketsVw> DatabasePluginPostgres::ticketsVw(bool all) {
    return ticketsVw(-1, all);
}


QList<Dbt::TicketsVw> DatabasePluginPostgres::ticketsVw(int ticket, bool all) {
    QList<Dbt::Tickets> list1 = tickets(ticket,all);
    QList<Dbt::TicketsVw> list;
    MSqlQuery q(m_db);
    for (int i=0; i<list1.size(); i++) {
        Dbt::TicketsVw x = list1[i];
        x.timesheets = ticketTimesheets(ticket, all);
        x.statuses = ticketStatus(ticket, all);
        x.values = ticketValues(ticket, all);
        x.files = ticketFiles(ticket, all);
        list << x;
        }

    return list;
}


void DatabasePluginPostgres::remove(const Dbt::Tickets& id) {
    MSqlQuery q(m_db);
    q.prepare(R"'(delete from ticketswhere category = :id;)'");
    q.bindValue(":id", id.ticket);
    q.exec();
}


QVariant DatabasePluginPostgres::save(const Dbt::Tickets& data) {
    MSqlQuery q(m_db);

    q.prepare(R"'(select 1 from tickets where ticket = :ticket;)'");
    q.bindValue(":ticket", data.ticket);
    q.exec();
    if (q.next()) {
        q.prepare(R"'(
            update tickets set
                category = :category,
                date = :date,
                price = :price,
                description = :description,
               "user" = :user
                where ticket = :ticket
            )'");
        q.bindValue(":category", data.category);
        q.bindValue(":date", data.date);
        q.bindValue(":price", data.price);
        q.bindValue(":description", data.description);
        q.bindValue(":user", data.user);
        q.bindValue(":ticket", data.ticket);
        q.exec();
        return QVariant(data.ticket);

      } else {

        q.prepare(R"'(
            insert into tickets (category, date, price, description, "user")
                        values (:category, :date, :price, :description, :user);
            )'");
        q.bindValue(":category", data.category);
        q.bindValue(":date", data.date);
        q.bindValue(":price", data.price);
        q.bindValue(":description", data.description);
        q.bindValue(":user", data.user);
        q.exec();
        return currval("tickets_ticket_seq");
        }

    return QVariant();

}


QVariant DatabasePluginPostgres::save(const Dbt::TicketsVw& data) {
    MSqlQuery q(m_db);
    q.begin();
    save(dynamic_cast<const Dbt::Tickets&>(data));
    save(data.timesheets);
    save(data.statuses);
    save(data.values);
    save(data.files);
    q.commit();

    return QVariant();
}


QList<Dbt::TicketStatus> DatabasePluginPostgres::ticketStatus(int ticket, bool all) {
    PDEBUG;
    createTemporaryTableTickets(ticket, all);
    QList<Dbt::TicketStatus> list;
    MSqlQuery q(m_db);

    q.prepare(R"'(
        select ts.id, ts.ticket, ts."user", ts.date, ts.description, ts.status
            from temporary_tickets t, ticket_status ts, users u
            where t.ticket = ts.ticket
        )'");
    q.exec();
    while (q.next()) {
        Dbt::TicketStatus x;
        int i=0;
        x.id            = q.value(i++).toInt();
        x.ticket        = q.value(i++).toInt();
        x.user          = q.value(i++).toInt();
        x.date          = q.value(i++).toDateTime();
        x.description   = q.value(i++).toString();
        x.status        = q.value(i++).toString();
        list << x;
        }
    return list;
}


QList<Dbt::TicketStatus> DatabasePluginPostgres::ticketStatus(bool all) {
    return ticketStatus(-1, all);
}


QList<Dbt::TicketStatus> DatabasePluginPostgres::ticketStatus(int id) {
    PDEBUG;
    QList<Dbt::TicketStatus> list;
    MSqlQuery q(m_db);

    q.prepare(R"'(
        select ts.id, ts.ticket, ts."user", ts.date, ts.description, ts.status
            from ticket_status ts, users u, tickets t, users_categories uc
            where ts.id = :id
              and t.ticket = ts.ticket
              and t.category = uc.category
              and u."user" = ts."user"
        )'");
    q.bindValue(":id", id);
    q.bindValue(":user", userId());
    q.exec();
    while (q.next()) {
        Dbt::TicketStatus x;
        int i=0;
        x.id            = q.value(i++).toInt();
        x.ticket        = q.value(i++).toInt();
        x.user          = q.value(i++).toInt();
        x.date          = q.value(i++).toDateTime();
        x.description   = q.value(i++).toString();
        x.status        = q.value(i++).toString();
        list << x;
        }
    return list;
}



void DatabasePluginPostgres::remove(const Dbt::TicketStatus& id) {
    MSqlQuery q(m_db);
    q.prepare(R"'(delete from ticket_status where id = :id;)'");
    q.bindValue(":id", id.id);
    q.exec();
}


QVariant DatabasePluginPostgres::save(const Dbt::TicketStatus& data) {
    MSqlQuery q(m_db);

    q.prepare(R"'(select 1 from ticket_status where id = :id;)'");
    q.bindValue(":id", data.id);
    q.exec();
    if (q.next()) {
        q.prepare(R"'(
            update ticket_status set
                ticket = :ticket,
               "user" = :user,
                date = :date,
                description = :description,
                status = :status
                where id = :id
            )'");
        q.bindValue(":id", data.id);
        q.bindValue(":user", data.user);
        q.bindValue(":ticket", data.ticket);
        q.bindValue(":date", data.date);
        q.bindValue(":description", data.description);
        q.bindValue(":status", data.status);
        q.exec();
        return QVariant(data.id);

      } else {

        q.prepare(R"'(
            insert into ticket_status (ticket, "user", date, description, status)
                values (:ticket, :user, :date, :description, :status)
            )'");
        q.bindValue(":user", data.user);
        q.bindValue(":ticket", data.ticket);
        q.bindValue(":date", data.date);
        q.bindValue(":description", data.description);
        q.bindValue(":status", data.status);
        q.exec();

        return currval("ticket_status_id_seq");
        }

    return QVariant();
}


QList<Dbt::TicketTimesheets> DatabasePluginPostgres::ticketTimesheets(int ticket, bool all) {
    createTemporaryTableTickets(ticket, all);
    QList<Dbt::TicketTimesheets> list;
    MSqlQuery q(m_db);
    q.prepare(R"'(
        select tt.id, tt.ticket, tt."user", tt.date_from, tt.date_to
            from temporary_tickets t, ticket_timesheets tt
            where t.ticket = tt.ticket
              and t.ticket = :ticket
            ;
        )'");
    q.bindValue(":user", userId());
    q.bindValue(":ticket", ticket);
    q.exec();
    while (q.next()) {
        int i=0;
        Dbt::TicketTimesheets x;
        x.id            = q.value(i++).toInt();
        x.ticket        = q.value(i++).toInt();
        x.user          = q.value(i++).toInt();
        x.date_from     = q.value(i++).toDateTime();
        x.date_to       = q.value(i++).toDateTime();
        list << x;
        }
    return list;
}


QList<Dbt::TicketTimesheets> DatabasePluginPostgres::ticketTimesheets(int id) {
    QList<Dbt::TicketTimesheets> list;
    MSqlQuery q(m_db);
    q.prepare(R"'(
        select tt.id, tt.ticket, tt."user", tt.date_from, tt.date_to
            from ticket_timesheets tt, tickets t, users_categories uc
            where t.ticket = tt.ticket
              and t.category = uc.category
              and uc."user" = :user
              and :id = tt.id
            ;
        )'");
    q.bindValue(":user", userId());
    q.bindValue(":id", id);
    q.exec();
    while (q.next()) {
        int i=0;
        Dbt::TicketTimesheets x;
        x.id            = q.value(i++).toInt();
        x.ticket        = q.value(i++).toInt();
        x.user          = q.value(i++).toInt();
        x.date_from     = q.value(i++).toDateTime();
        x.date_to       = q.value(i++).toDateTime();
        list << x;
        }
    return list;
}


QList<Dbt::TicketTimesheets> DatabasePluginPostgres::ticketTimesheets(bool all) {
    return ticketTimesheets(-1, all);
}


QVariant DatabasePluginPostgres::save(const Dbt::TicketTimesheets& data) {
    MSqlQuery q(m_db);

    q.prepare(R"'(select 1 from ticket_timesheets where id = :id;)'");
    q.bindValue(":id", data.id);
    q.exec();
    if (q.next()) {
        q.prepare(R"'(
            update ticket_timesheets set
                ticket = :ticket,
               "user" = :user,
                date_from = :date_from,
                date_to = :date_to
                where id = :id
            )'");
        q.bindValue(":id", data.id);
        q.bindValue(":user", data.user);
        q.bindValue(":ticket", data.ticket);
        q.bindValue(":date_from", data.date_from);
        q.bindValue(":date_to", data.date_to);
        q.exec();
        return QVariant(data.id);

       } else {

        q.prepare(R"'(
            insert into ticket_timesheets (ticket, "user", date_from, date_to)
                values (:ticket, :user, :date_from, :date_to)
            )'");
        q.bindValue(":user", data.user);
        q.bindValue(":ticket", data.ticket);
        q.bindValue(":date_from", data.date_from);
        q.bindValue(":date_to", data.date_to);
        q.exec();

        return currval("ticket_timesheets_id_seq");
        }

    return QVariant();
}


void DatabasePluginPostgres::remove(const Dbt::TicketTimesheets& id) {
    MSqlQuery q(m_db);
    q.prepare(R"'(delete from ticket_timesheets where id = :id;)'");
    q.bindValue(":id", id.id);
    q.exec();
}


QList<Dbt::TicketValues> DatabasePluginPostgres::ticketValues(int ticket, bool all) {
    createTemporaryTableTickets(ticket, all);
    QList<Dbt::TicketValues> list;
    MSqlQuery q(m_db);
    q.prepare(R"'(
        select tv.id, tt.ticket, tv.name, tv.value, tv."user", tv.date
            from temporary_tickets tt, ticket_values tv
            where tt.ticket = tv.ticket
            ;
        )'");
    q.exec();
    while (q.next()) {
        int i=0;
        Dbt::TicketValues x;
        x.id            = q.value(i++).toInt();
        x.ticket        = q.value(i++).toInt();
        x.name          = q.value(i++).toString();
        x.value         = q.value(i++).toString();
        x.user          = q.value(i++).toInt();
        x.date          = q.value(i++).toDateTime();
        list << x;
        }
    return list;
}


QList<Dbt::TicketValues> DatabasePluginPostgres::ticketValues(int id) {
    QList<Dbt::TicketValues> list;
    MSqlQuery q(m_db);
    q.prepare(R"'(
        select tv.id, tt.ticket, tt.name, tt.value, tv."user"
            from tickets tt, ticket_values tv, user_categories uc
            where tt.ticket = tv.ticket
              and tt.category = uc.category
              and uc.user = :user
              and :id = id
            ;
        )'");
    q.bindValue(":id", id);
    q.bindValue(":user", userId());
    q.exec();
    while (q.next()) {
        int i=0;
        Dbt::TicketValues x;
        x.id            = q.value(i++).toInt();
        x.ticket        = q.value(i++).toInt();
        x.name          = q.value(i++).toString();
        x.value         = q.value(i++).toString();
        x.user          = q.value(i++).toInt();
        list << x;
        }
    return list;
}


QList<Dbt::TicketValues> DatabasePluginPostgres::ticketValues(bool all) {
    return ticketValues(-1, all);
}


QVariant DatabasePluginPostgres::save(const Dbt::TicketValues& data) {
    MSqlQuery q(m_db);

    q.prepare(R"'(select 1 from ticket_values where id = :id;)'");
    q.bindValue(":id", data.id);
    q.exec();
    if (q.next()) {
        q.prepare(R"'(
            update ticket_values set
                ticket = :ticket,
                name = :name,
                value = :value
                where id = :id
            )'");
        q.bindValue(":id", data.id);
        q.bindValue(":name", data.name);
        q.bindValue(":value", data.value);
        q.bindValue(":ticket", data.ticket);
        q.exec();
        return QVariant(data.id);

      } else {

        q.prepare(R"'(
            insert into ticket_values (ticket, name, value)
                select :ticket, :name, :value
                where not exists (select 1 from ticket_values where id = :id);
            )'");
        q.bindValue(":id", data.id);
        q.bindValue(":name", data.name);
        q.bindValue(":value", data.value);
        q.bindValue(":ticket", data.ticket);
        q.exec();

        return currval("ticket_values_id_seq");
        }

    return QVariant();
}


void DatabasePluginPostgres::remove(const Dbt::TicketValues& id) {
    MSqlQuery q(m_db);
    q.prepare(R"'(delete from ticket_values where id = :id;)'");
    q.bindValue(":id", id.id);
    q.exec();
}


QList<Dbt::Statuses> DatabasePluginPostgres::statuses(const QString& id) {
    QList<Dbt::Statuses> list;
    MSqlQuery q(m_db);
    if (id.isEmpty() || id == "") {
        q.prepare(R"'(
            select status, description, abbreviation, color, closed
            from statuses
            )'");
      } else {
        q.prepare(R"'(
            select status, description, abbreviation, color, closed
            from statuses
            where (:id = status);
        )'");
        }
    q.bindValue(":id", id);
    q.exec();
    while (q.next()) {
        int i=0;
        Dbt::Statuses x;
        x.status = q.value(i++).toString();
        x.description = q.value(i++).toString();
        x.abbreviation = q.value(i++).toString();
        x.color = q.value(i++).toString();
        x.closed = q.value(i++).toBool();
        list << x;
        }
    return list;
}


void DatabasePluginPostgres::remove(const Dbt::Statuses& id) {
    MSqlQuery q(m_db);
    q.prepare(R"'(delete from statuses where status = :id;)'");
    q.bindValue(":id", id.status);
    q.exec();
}


QVariant DatabasePluginPostgres::save(const Dbt::Statuses& data) {
    MSqlQuery q(m_db);

    q.prepare(R"'(
        update statuses set
            description = :description,
            abbreviation = :abbreviation,
            color = :color,
            closed = :closed
            where status = :status
        )'");
    q.bindValue(":description", data.description);
    q.bindValue(":abbreviation", data.abbreviation);
    q.bindValue(":color", data.color);
    q.bindValue(":closed", data.closed);
    q.bindValue(":status", data.status);
    q.exec();

    q.prepare(R"'(
        insert into statuses (status, description, abbreviation, color, closed)
            select :status1, :description, :abbreviation, :color, :closed
            where not exists (select 1 from statuses where status = :status2);
        )'");
    q.bindValue(":status1", data.status);
    q.bindValue(":description", data.description);
    q.bindValue(":abbreviation", data.abbreviation);
    q.bindValue(":color", data.color);
    q.bindValue(":closed", data.closed);
    q.bindValue(":status2", data.status);
    q.exec();

    return QVariant(data.status);
}


QList<Dbt::TicketFiles> DatabasePluginPostgres::ticketFiles(int ticket, bool all) {
    createTemporaryTableTickets(ticket, all);
    QList<Dbt::TicketFiles> list;
    MSqlQuery q(m_db);

    q.prepare(R"'(
        select f.id, f.ticket, f.name, f.type, f.content
            from temporary_tickets t, ticket_files f
            where t.ticket = f.ticket
            ;
        )'");
    q.exec();
    while (q.next()) {
        int i=0;
        Dbt::TicketFiles x;
        x.id        = q.value(i++).toInt();
        x.ticket    = q.value(i++).toInt();
        x.name      = q.value(i++).toString();
        x.type      = q.value(i++).toString();
        x.content   = q.value(i++).toByteArray();
        list << x;
        }

    return list;
}


QList<Dbt::TicketFiles> DatabasePluginPostgres::ticketFiles(int id) {
    QList<Dbt::TicketFiles> list;
    MSqlQuery q(m_db);

    q.prepare(R"'(
        select f.id, f.ticket, f.name, f.type, f.content
            from ticket_files f, users u, tickets t, users_categories us
            where ts.id = :id
              and t.ticket = ts.ticket
              and t.category = uc.category
              and u."user" = ts."user"
            ;
        )'");
    q.bindValue(":id", id);
    q.bindValue(":user", userId());
    q.exec();
    while (q.next()) {
        int i=0;
        Dbt::TicketFiles x;
        x.id        = q.value(i++).toInt();
        x.ticket    = q.value(i++).toInt();
        x.name      = q.value(i++).toString();
        x.type      = q.value(i++).toString();
        x.content   = q.value(i++).toByteArray();
        list << x;
        }

    return list;
}


QList<Dbt::TicketFiles> DatabasePluginPostgres::ticketFiles(bool all) {
    return ticketFiles(-1, all);
}


QVariant DatabasePluginPostgres::save(const Dbt::TicketFiles& data) {
    MSqlQuery q(m_db);

    q.prepare(R"'(select 1 from ticket_files where id = :id;)'");
    q.bindValue(":id", data.id);
    q.exec();
    if (q.next()) {
        q.prepare(R"'(
            update ticket_files set
                ticket = :ticket,
                name = :name,
                type = :type,
                content = :content
                where id = :id
            )'");
        q.bindValue(":id", data.id);
        q.bindValue(":ticket", data.ticket);
        q.bindValue(":name", data.name);
        q.bindValue(":type", data.type);
        q.bindValue(":content", data.content);
        q.exec();
        return QVariant(data.id);

      } else {

        q.prepare(R"'(
            insert into ticket_files (ticket, name, type, content)
                values (:ticket, :name, :type, :content);
            )'");
        q.bindValue(":id", data.id);
        q.bindValue(":ticket", data.ticket);
        q.bindValue(":name", data.name);
        q.bindValue(":type", data.type);
        q.bindValue(":content", data.content);
        q.exec();
        return currval("ticket_files_id_seq");
        }

    return QVariant();
}


void DatabasePluginPostgres::remove(const Dbt::TicketFiles& id) {
    MSqlQuery q(m_db);
    q.prepare(R"'(delete from ticket_files where id = :id;)'");
    q.bindValue(":id", id.id);
    q.exec();
}


