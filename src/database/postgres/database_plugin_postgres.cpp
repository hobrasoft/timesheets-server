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
#include <QRandomGenerator>

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
    PDEBUG << login << password;
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

    if (parent_category.toInt() == category.toInt()) {
        parent_category = QVariant();
        }

    // TODO: Kontrola, aby nešlo založit novou kategorii v nadřízené kategorii bez přístupu

    q.prepare(R"'(select 1 from categories where category = :category;)'");
    q.bindValue(":category", data.category);
    q.exec();
    if (q.next() && ! parent_category.isNull()) {
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
        q.bindValue(3, category.toInt());
        q.exec();
        return QVariant(data.category);
        }
    if (q.next() && parent_category.isNull()) {
        q.prepare(R"'(
            update categories set
                description = ?,
                price = ?
                where category = ?
            )'");
        q.bindValue(1, data.description);
        q.bindValue(2, data.price);
        q.bindValue(3, category.toInt());
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


QList<Dbt::ClientSettings> DatabasePluginPostgres::clientSettings() {
    QList<Dbt::ClientSettings> list;
    MSqlQuery q(m_db);

    q.prepare(R"'(
        select multiple_timesheets, show_price, can_change_category, edit_categories, 
               show_multiple_timesheets, show_show_price, show_can_change_category, show_edit_categories
        from client_settings;
        )'");
    q.exec();
    while (q.next()) {
        Dbt::ClientSettings x;
        int i = 0;
        x.multiple_timesheets = q.value(i++).toBool(); 
        x.show_price = q.value(i++).toBool(); 
        x.can_change_category = q.value(i++).toBool(); 
        x.edit_categories = q.value(i++).toBool(); 
        x.show_multiple_timesheets = q.value(i++).toBool(); 
        x.show_show_price = q.value(i++).toBool(); 
        x.show_can_change_category = q.value(i++).toBool(); 
        x.show_edit_categories = q.value(i++).toBool(); 
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


QList<Dbt::Categories> DatabasePluginPostgres::categoriesToRoot(const QString& id) {
    QList<Dbt::Categories> list;
    MSqlQuery q(m_db);
    int xid = id.toInt();

    q.prepare(R"'(
        select c.category, c.parent_category, c.description, c.price 
        from categories c, users_categories uc
        where c.category = uc.category
          and uc."user" = :user
          and :id = c.category;
        )'");
    for (;;) {
        q.bindValue(":user", userId());
        q.bindValue(":id", xid);
        q.exec();
        bool found = q.next();
        PDEBUG << xid << found;
        if (!found) { return list; }
        int i=0;
        Dbt::Categories x;
        x.category = q.value(i++).toString();
        x.parent_category = null(q.value(i++).toString());
        x.description = q.value(i++).toString();
        x.price = q.value(i++).toDouble();
        list.prepend(x);
        xid = x.parent_category.toInt();
        }

    return list;
}


QList<Dbt::Categories> DatabasePluginPostgres::subcategories(const QString& id) {
    QList<Dbt::Categories> list;
    MSqlQuery q(m_db);

    q.prepare(R"'(
        select c.category, c.parent_category, c.description, c.price 
        from categories c, users_categories uc
        where c.category = uc.category
          and uc."user" = :user
          and ((:id1 <= 0 and c.parent_category is null) or :id2 = c.parent_category);
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
        PDEBUG << "Vybiram VSE" << ticket;
        q.prepare(R"'(
            insert into temporary_tickets (ticket, category, date, price, description, "user")
            select t.ticket, t.category, t.date, t.price, t.description, t."user"
                from tickets t, users u
                where (t.category in (select category from users_categories where "user" = u."user") or u.admin = true)
                  and u."user" = :user
                  and (:ticket1 <= 0 or :ticket2 = t.ticket)
            ;
            )'");
      } else {
        PDEBUG << "Vybiram pouze otevrene" << ticket;
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

    q.exec("select count(1) from temporary_tickets;");
    q.next();
    PDEBUG << "Vybranych vet" << q.value(0).toInt();

}


QList<Dbt::Tickets> DatabasePluginPostgres::tickets(bool all) {
    return tickets(-1, all);
}


QList<Dbt::Tickets> DatabasePluginPostgres::tickets(int ticket, bool all) {
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
        x.price         = q.value(i++).toDouble();
        x.description   = q.value(i++).toString();
        x.user          = q.value(i++).toInt();
        list << x;
        }
    return list;
}


QList<Dbt::Tickets> DatabasePluginPostgres::tickets(const Dbt::Categories& category, bool all) {
    createTemporaryTableTickets(-1, all);
    QList<Dbt::Tickets> list;
    MSqlQuery q(m_db);

    q.prepare(R"'(
        select ticket, category, date, price, description, "user"
            from temporary_tickets 
           where category = :category;
        )'");
    q.bindValue(":category", category.category);
    q.exec();
    while (q.next()) {
        Dbt::Tickets x;
        int i=0;
        x.ticket        = q.value(i++);
        x.category      = q.value(i++);
        x.date          = q.value(i++).toDateTime();
        x.price         = q.value(i++).toDouble();
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
        x.timesheets = ticketTimesheets(list1[i].ticket.toInt(), all);
        x.statuses = ticketStatus(list1[i].ticket.toInt(), all);
        x.values = ticketValues(list1[i].ticket.toInt(), all);
        x.files = ticketFiles(list1[i].ticket.toInt(), all);
        list << x;
        }

    return list;
}


QList<Dbt::TicketsVw> DatabasePluginPostgres::ticketsVw(const Dbt::Categories& category, bool all) {
    QList<Dbt::Tickets> list1 = tickets(category, all);
    QList<Dbt::TicketsVw> list;
    MSqlQuery q(m_db);
    for (int i=0; i<list1.size(); i++) {
        Dbt::TicketsVw x = list1[i];
        x.timesheets = ticketTimesheets(list1[i].ticket.toInt(), true);
        x.statuses = ticketStatus(list1[i].ticket.toInt(), true);
        x.values = ticketValues(list1[i].ticket.toInt(), true);
        x.files = ticketFiles(list1[i].ticket.toInt(), true);
        list << x;
        }

    return list;
}


void DatabasePluginPostgres::remove(const Dbt::Tickets& id) {
    MSqlQuery q(m_db);
    q.prepare(R"'(delete from tickets where ticket = :id;)'");
    q.bindValue(":id", id.ticket);
    q.exec();
}


QVariant DatabasePluginPostgres::save(const Dbt::Tickets& data) {
    MSqlQuery q(m_db);

    bool found = false;
    if (!data.created) {
        q.prepare(R"'(select 1 from tickets where ticket = :ticket;)'");
        q.bindValue(":ticket", data.ticket);
        q.exec();
        found = q.next();
        }

    if (!data.created && found) {
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
        }

    if (data.created || !found) {
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

    Q_UNREACHABLE();
    qFatal("Should not happen");
    return QVariant();

}


template<typename T>
QList<T> remapTicket(const QList<T>& input, int ticket) {
    QList<T> list;
    QListIterator<T> iterator(input);
    while (iterator.hasNext()) {
        T x = iterator.next();
        x.ticket = ticket;
        if (ticket <= 0) { x.id = 0; }
        list << x;
        }
    return list;
}


QVariant DatabasePluginPostgres::save(const Dbt::TicketsVw& data) {
    MSqlQuery q(m_db);
    q.begin();
    int ticket = save(dynamic_cast<const Dbt::Tickets&>(data)).toInt();
    removeTicketsDetails(ticket);
    save(remapTicket(data.timesheets, ticket));
    save(remapTicket(data.statuses, ticket));
    save(remapTicket(data.values, ticket));
    save(remapTicket(data.files, ticket));
    q.commit();

    return QVariant();
}


void DatabasePluginPostgres::removeTicketsDetails(int ticket) {
    MSqlQuery q(m_db);
    q.exec(QString(R"'(delete from ticket_timesheets where ticket = %1;)'").arg(ticket));
    q.exec(QString(R"'(delete from ticket_status where ticket = %1;)'").arg(ticket));
    q.exec(QString(R"'(delete from ticket_values where ticket = %1;)'").arg(ticket));
    q.exec(QString(R"'(delete from ticket_files where ticket = %1;)'").arg(ticket));
}


QList<Dbt::TicketStatus> DatabasePluginPostgres::ticketStatus(int ticket, bool all) {
    createTemporaryTableTickets(ticket, all);
    QList<Dbt::TicketStatus> list;
    MSqlQuery q(m_db);

    q.prepare(R"'(
        select ts.id, ts.ticket, ts."user", ts.date, ts.description, ts.status, s.color, s.description, s.closed, s.can_be_run, s.ignored
            from temporary_tickets t, ticket_status ts, statuses s
            where t.ticket = ts.ticket
              and ts.status = s.status
              and t.ticket = :ticket
        )'");
    q.bindValue(":ticket", ticket);
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
        x.status_color  = q.value(i++).toString();
        x.status_description = q.value(i++).toString();
        x.status_closed = q.value(i++).toBool();
        x.status_can_be_run = q.value(i++).toBool();
        x.status_ignored    = q.value(i++).toBool();
        list << x;
        }
    return list;
}


QList<Dbt::TicketStatus> DatabasePluginPostgres::ticketStatus(bool all) {
    return ticketStatus(-1, all);
}


QList<Dbt::TicketStatus> DatabasePluginPostgres::ticketStatus(int id) {
    QList<Dbt::TicketStatus> list;
    MSqlQuery q(m_db);

    q.prepare(R"'(
        select ts.id, ts.ticket, ts."user", ts.date, ts.description, ts.status, s.color, s.description, s.close, s.can_be_run, s.ignored
            from ticket_status ts, users u, tickets t, users_categories uc, statuses s
            where ts.id = :id
              and t.ticket = ts.ticket
              and t.category = uc.category
              and u."user" = ts."user"
              and ts.status = s.status
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
        x.status_color  = q.value(i++).toString();
        x.status_description = q.value(i++).toString();
        x.status_closed = q.value(i++).toBool();
        x.status_can_be_run = q.value(i++).toBool();
        x.status_ignored    = q.value(i++).toBool();
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

    bool found = false;
    if (!data.created) {
        q.prepare(R"'(select 1 from ticket_status where id = :id;)'");
        q.bindValue(":id", data.id);
        q.exec();
        found = q.next();
        }

    if (!data.created && found) {
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
        }

    if (data.created || !found) {
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

    Q_UNREACHABLE();
    qFatal("Should not happen");
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
              order by tt.date_from -- must be sorted!
            ;
        )'");
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
            order by tt.date_from
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


QList<Dbt::TicketTimesheets> DatabasePluginPostgres::runningTimesheets(int ticket) {
    QList<Dbt::TicketTimesheets> list;
    MSqlQuery q(m_db);
    q.prepare(R"'(
        select tt.id, tt.ticket, tt."user", tt.date_from, tt.date_to
            from ticket_timesheets tt, tickets t, users_categories uc
            where t.ticket = tt.ticket
              and t.category = uc.category
              and uc."user" = :user
              and tt.date_to is null
              and (:ticket1 = tt.ticket or :ticket2 <= 0)
            ;
        )'");
    q.bindValue(":user", userId());
    q.bindValue(":ticket1", ticket);
    q.bindValue(":ticket2", ticket);
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


QList<Dbt::TicketTimesheets> DatabasePluginPostgres::startTimesheet(int ticket) {
    QList<Dbt::TicketTimesheets> list;
    MSqlQuery q(m_db);

    // Zkontroluj, jestli je ticket dostupný pro uživatele a je zastavený
    q.prepare(R"'(
        select 1
            from ticket_timesheets tt, tickets t, users_categories uc
            where t.ticket = tt.ticket
              and t.category = uc.category
              and uc."user" = :user
              and :ticket = tt.ticket
              and tt.date_to is null
            ;
        )'");
    q.bindValue(":user", userId());
    q.bindValue(":ticket", ticket);
    q.exec();
    if (q.next()) {
        return list;
        }

    // Zkontroluj, jestli je ticket dostupný pro uživatele a existující
    q.prepare(R"'(
        select 1 
            from tickets t, users_categories uc
            where t.category = uc.category
              and uc."user" = :user
              and :ticket = t.ticket
            ;
        )'");
    q.bindValue(":user", userId());
    q.bindValue(":ticket", ticket);
    q.exec();
    if (!q.next()) {
        return list;
        }

    // Vložit timesheet
    q.prepare(R"'(
        insert into ticket_timesheets (ticket, "user", date_from) 
            values (:ticket, :user, now())
        )'");
    q.bindValue(":user", userId());
    q.bindValue(":ticket", ticket);
    q.exec();
    QVariant id = currval("ticket_timesheets_id_seq").toInt();
    if (id.isNull() || !id.isValid()) {
        return list;
        }

    list = ticketTimesheets(id.toInt());
    return list;
}
    

QList<Dbt::TicketTimesheets> DatabasePluginPostgres::stopTimesheet(int ticket) {
    QList<Dbt::TicketTimesheets> list;
    MSqlQuery q(m_db);

    q.begin();
    // Zkontroluj, jestli je ticket dostupný pro uživatele a je běžící
    q.prepare(R"'(
        select tt.id 
            from ticket_timesheets tt, tickets t, users_categories uc
            where t.ticket = tt.ticket
              and t.category = uc.category
              and uc."user" = :user
              and :ticket = tt.ticket
              and tt.date_to is null
            ;
        )'");
    q.bindValue(":user", userId());
    q.bindValue(":ticket", ticket);
    q.setForwardOnly(false);
    q.exec();
    if (q.size() != 1) {
        q.commit();
        return list;
        }
    q.next();
    int id = q.value(0).toInt();

    // Ukončit timesheet
    q.prepare(R"'(
        update ticket_timesheets set date_to = now() where id = :id;
        )'");
    q.bindValue(":id", id);
    q.exec();

    list = ticketTimesheets(ticket, true);

    int remove_secs = 10;
    int round2_mins = 5;
    int round5_mins = 5;
    int join_mins = 5;
    int remove_singles_mins = 3;

    // step 1 - remove timesheets shorter than ...
    for (int i=list.size()-1; i>=0; i--) {
        Dbt::TicketTimesheets& x = list[i];
        if (x.date_from.secsTo(x.date_to) > remove_secs) { continue; }
        list.removeAt(i);
        }

    // step2 - round timeshiits to ...
    for (int i=list.size()-1; i>=0; i--) {
        Dbt::TicketTimesheets& x = list[i];
        int secs;
        secs = x.date_from.secsTo(x.date_to);
        secs = secs / (round2_mins * 60) + ( (secs % (round2_mins*60)) ? 1 : 0);
        secs = secs * (round2_mins * 60);
        x.date_to = x.date_from.addSecs(secs);
        }

    // step3 - join overlapinng records
    for (int i=list.size()-1; i>=1; i--) {
        Dbt::TicketTimesheets& x0 = list[i-1];
        Dbt::TicketTimesheets& x1 = list[i];

        if (x0.date_to < x1.date_from) { continue; }
        x0.date_to = x1.date_to;
        list.removeAt(i);
        }

    // step4 - join near records
    for (int i=list.size()-1; i>=1; i--) {
        Dbt::TicketTimesheets& x0 = list[i-1];
        Dbt::TicketTimesheets& x1 = list[i];
        if (x0.date_to.addSecs(join_mins*60) < x1.date_from) { continue; }
        x0.date_to = x1.date_to;
        list.removeAt(i);
        }

    // step5  - round timetracks to ...
    for (int i=list.size()-1; i>=0; i--) {
        Dbt::TicketTimesheets& x = list[i];
        int secs;
        secs = x.date_from.secsTo(x.date_to);
        secs = secs + (round5_mins * 60) -1;
        secs = secs / (round5_mins * 60);
        secs = secs * (round5_mins * 60);
        x.date_to = x.date_from.addSecs(secs);
        }

    // step6 - remove short single records
    for (int i=list.size()-1; i>=0; i--) {
        Dbt::TicketTimesheets& x = list[i];
        int secs = x.date_from.secsTo(x.date_to);
        if (secs <= 0) { continue; }
        if (secs >= remove_singles_mins * 60) { continue; }
        list.removeAt(i);
        }

    q.prepare(R"'(delete from ticket_timesheets where ticket = :ticket;)'");
    q.bindValue(":ticket", ticket);
    q.exec();

    q.prepare(R"'(insert into ticket_timesheets (ticket, "user", date_from, date_to) 
            values (:ticket, :user, :date_from, :date_to);
        )'");
    for (int i=0; i<list.size(); i++) {
        Dbt::TicketTimesheets& x = list[i];
        q.bindValue(":ticket", ticket);
        q.bindValue(":user", userId());
        q.bindValue(":date_from", x.date_from);
        q.bindValue(":date_to", x.date_to);
        q.exec();
        }

    q.commit();
    return list.mid(list.size()-1);
}


QList<Dbt::TicketTimesheets> DatabasePluginPostgres::toggleTimesheet(int ticket) {
    QList<Dbt::TicketTimesheets> list;
    MSqlQuery q(m_db);

    // Zkontroluj, jestli je ticket dostupný pro uživatele a existující
    q.prepare(R"'(
        select 1 
            from tickets t, users_categories uc
            where t.category = uc.category
              and uc."user" = :user
              and :ticket = t.ticket
            ;
        )'");
    q.bindValue(":user", userId());
    q.bindValue(":ticket", ticket);
    q.exec();
    if (!q.next()) {
        return list;
        }

    // Zkontroluj, jestli je ticket dostupný pro uživatele a je běžící
    q.prepare(R"'(
        select tt.id 
            from ticket_timesheets tt, tickets t, users_categories uc
            where t.ticket = tt.ticket
              and t.category = uc.category
              and uc."user" = :user
              and :ticket = tt.ticket
              and tt.date_to is null
            ;
        )'");
    q.bindValue(":user", userId());
    q.bindValue(":ticket", ticket);
    q.setForwardOnly(false);
    q.exec();
    bool found = (q.size() == 1);
    int id = (found) ? q.next(), q.value(0).toInt() : 0;

    if (!found) {
        // Vložit timesheet
        q.prepare(R"'(
            insert into ticket_timesheets (ticket, "user", date_from)
                values (:ticket, :user, now())
            )'");
        q.bindValue(":user", userId());
        q.bindValue(":ticket", ticket);
        q.exec();
        QVariant newid = currval("ticket_timesheets_id_seq");
        if (newid.isNull() || !newid.isValid()) {
            return list;
            }
        id = newid.toInt();
        }

    if (found) {
        // Ukončit timesheet
        q.prepare(R"'(
            update ticket_timesheets set date_to = now() where id = :id;
            )'");
        q.bindValue(":id", id);
        q.exec();
        }

    list = ticketTimesheets(id);
    return list;
}


QVariant DatabasePluginPostgres::save(const Dbt::TicketTimesheets& data) {
    MSqlQuery q(m_db);

    bool found = false;
    if (!data.created) {
        q.prepare(R"'(select 1 from ticket_timesheets where id = :id;)'");
        q.bindValue(":id", data.id);
        q.exec();
        found = q.next();
        }

    if (!data.created && found) {
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
        }

    if (data.created || !found) {
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

    Q_UNREACHABLE();
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
        x.value         = JSON::data(q.value(i++).toByteArray());
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
        select tv.id, tv.ticket, tv.date, tv.name, tv.value, tv."user"
            from tickets tt, ticket_values tv, users_categories uc
            where tt.ticket = tv.ticket
              and tt.category = uc.category
              and uc.user = :user
              and :id = tv.id
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
        x.date          = q.value(i++).toDateTime();
        x.name          = q.value(i++).toString();
        x.value         = JSON::data(q.value(i++).toByteArray());
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

    bool found = false;
    if (!data.created) {
        q.prepare(R"'(select 1 from ticket_values where id = :id;)'");
        q.bindValue(":id", data.id);
        q.exec();
        found = q.next();
        }

    if (!data.created && found) {
        q.prepare(QString(R"'(
            update ticket_values set
                ticket = :ticket,
               "user" = :user,
                date = :date,
                name = :name,
                value = '%1'
                where id = :id
            )'").arg(QString::fromUtf8(JSON::json(data.value))));
        q.bindValue(":id", data.id);
        q.bindValue(":ticket", data.ticket);
        q.bindValue(":user", data.user);
        q.bindValue(":date", data.date);
        q.bindValue(":name", data.name);
        // q.bindValue(":value", JSON::json(data.value));
        q.exec();
        return QVariant(data.id);
        }

    if (data.created || !found) {
        q.prepare(QString(R"'(
            insert into ticket_values (ticket, "user", date, name, value)
                select :ticket, :user, :date, :name, '%1'
                where not exists (select 1 from ticket_values where id = :id);
            )'").arg(QString::fromUtf8(JSON::json(data.value))));
        q.bindValue(":id", data.id);
        q.bindValue(":ticket", data.ticket);
        q.bindValue(":user", data.user);
        q.bindValue(":date", data.date);
        q.bindValue(":name", data.name);
        // q.bindValue(":value", JSON::json(data.value));
        q.exec();

        return currval("ticket_values_id_seq");
        }

    Q_UNREACHABLE();
    qFatal("Should not happen");
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
            select status, description, abbreviation, color, closed, can_be_run, ignored
            from statuses
            )'");
      } else {
        q.prepare(R"'(
            select status, description, abbreviation, color, closed, can_be_run, ignored
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
        x.can_be_run = q.value(i++).toBool();
        x.ignored = q.value(i++).toBool();
        list << x;
        }
    return list;
}


QList<Dbt::Statuses> DatabasePluginPostgres::statuses(const QString& category, const QString& previousStatus) {
    if ((category.isEmpty() || category == "") && 
        (previousStatus.isEmpty() || previousStatus == "")) { return statuses(QString()); }
    int categoryi = category.toInt();
    MSqlQuery q(m_db);
    QString ps = (previousStatus.isEmpty()) ? "0" : previousStatus;
    auto results = [&q]() {
        QList<Dbt::Statuses> list;
        q.exec();
        while (q.next()) {
            Dbt::Statuses x;
            int i=0;
            x.status = q.value(i++).toString();
            x.description = q.value(i++).toString();
            x.abbreviation = q.value(i++).toString();
            x.color = q.value(i++).toString();
            x.closed = q.value(i++).toBool();
            x.can_be_run = q.value(i++).toBool();
            x.ignored = q.value(i++).toBool();
            list << x;
            }
        return list;
        };

    bool findNullCategory = (category.isEmpty() || category == "");
    if (!findNullCategory) {
        q.prepare(R"'(select 1 from status_order where category = :category;)'");
        q.bindValue(":category", categoryi);
        q.exec();
        findNullCategory = !q.next();
        }

    if (findNullCategory && (previousStatus.isEmpty() || previousStatus == "")) {
        q.prepare(R"'(
            select s.status, s.description, s.abbreviation, s.color, s.closed, s.can_be_run, s.ignored
            from statuses s, status_order o
            where s.status = o.next_status
            and (o.previous_status is null or o.previous_status = '')
            and (o.category is null);
            )'");
        return results();
        }

    if (findNullCategory) {
        q.prepare(R"'(
            select s.status, s.description, s.abbreviation, s.color, s.closed, s.can_be_run, s.ignored
            from statuses s, status_order o
            where s.status = o.next_status
            and o.previous_status = :previous_status
            and (o.category is null);
            )'");
        q.bindValue(":previous_status", ps);
        return results();
        }

    if (!findNullCategory && (previousStatus.isEmpty() || previousStatus == "")) {
        q.prepare(R"'(
            select s.status, s.description, s.abbreviation, s.color, s.closed, s.can_be_run, s.ignored
            from statuses s, status_order o
            where s.status = o.next_status
            and (o.previous_status is null or o.previous_status = '')
            and o.category = :category;
            )'");
        q.bindValue(":category", categoryi);
        return results();
        }


    if (!findNullCategory) {
        q.prepare(R"'(
            select s.status, s.description, s.abbreviation, s.color, s.closed, s.can_be_run, s.ignored
            from statuses s, status_order o
            where s.status = o.next_status
            and o.previous_status = :previous_status
            and o.category = :category;
            )'");
        q.bindValue(":previous_status", ps);
        q.bindValue(":category", categoryi);
        return results();
        }

    return QList<Dbt::Statuses>();

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
            can_be_run = :can_be_run 
            ignored = :ignored
            where status = :status
        )'");
    q.bindValue(":description", data.description);
    q.bindValue(":abbreviation", data.abbreviation);
    q.bindValue(":color", data.color);
    q.bindValue(":closed", data.closed);
    q.bindValue(":can_be_run", data.can_be_run);
    q.bindValue(":ignored", data.ignored);
    q.bindValue(":status", data.status);
    q.exec();

    q.prepare(R"'(
        insert into statuses (status, description, abbreviation, color, closed, can_be_run, ignored)
            select :status1, :description, :abbreviation, :color, :closed, :can_be_run, :ignored
            where not exists (select 1 from statuses where status = :status2);
        )'");
    q.bindValue(":status1", data.status);
    q.bindValue(":description", data.description);
    q.bindValue(":abbreviation", data.abbreviation);
    q.bindValue(":color", data.color);
    q.bindValue(":closed", data.closed);
    q.bindValue(":can_be_run", data.can_be_run);
    q.bindValue(":ignored", data.ignored);
    q.bindValue(":status2", data.status);
    q.exec();

    return QVariant(data.status);
}


QList<Dbt::StatusTemplates> DatabasePluginPostgres::statusTemplates(int id) {
    MSqlQuery q(m_db);
    QList<Dbt::StatusTemplates> list;
    return list;
}


QList<Dbt::TicketFiles> DatabasePluginPostgres::ticketFiles(int ticket, bool all) {
    createTemporaryTableTickets(ticket, all);
    QList<Dbt::TicketFiles> list;
    MSqlQuery q(m_db);

    q.prepare(R"'(
        select f.id, f."user", f.date, f.ticket, f.name, f.type, f.content
            from temporary_tickets t, ticket_files f
            where t.ticket = f.ticket
            ;
        )'");
    q.exec();
    while (q.next()) {
        int i=0;
        Dbt::TicketFiles x;
        x.id        = q.value(i++).toInt();
        x.user      = q.value(i++).toInt();
        x.date      = q.value(i++).toDateTime();
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
        select f.id, f."user", f.date, f.ticket, f.name, f.type, f.content
            from ticket_files f, users u, tickets t, users_categories uc
            where f.id = :id
              and t.ticket = f.ticket
              and t.category = uc.category
              and u."user" = f."user"
            ;
        )'");
    q.bindValue(":id", id);
    q.bindValue(":user", userId());
    q.exec();
    while (q.next()) {
        int i=0;
        Dbt::TicketFiles x;
        x.id        = q.value(i++).toInt();
        x.user      = q.value(i++).toInt();
        x.date      = q.value(i++).toDateTime();
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

    bool found = false;
    if (!data.created) {
        q.prepare(R"'(select 1 from ticket_files where id = :id;)'");
        q.bindValue(":id", data.id);
        q.exec();
        found = q.next();
        }

    if (!data.created && found) {
        q.prepare(R"'(
            update ticket_files set
                ticket = :ticket,
                date = :date,
               "user" = :user,
                name = :name,
                type = :type,
                content = :content
                where id = :id
            )'");
        q.bindValue(":id", data.id);
        q.bindValue(":ticket", data.ticket);
        q.bindValue(":user", data.user);
        q.bindValue(":date", data.date);
        q.bindValue(":name", data.name);
        q.bindValue(":type", data.type);
        q.bindValue(":content", data.content);
        q.exec();
        return QVariant(data.id);
        }

    if (data.created || !found) {
        q.prepare(R"'(
            insert into ticket_files (ticket, "user", date, name, type, content)
                values (:ticket, :user, :date, :name, :type, :content);
            )'");
        q.bindValue(":id", data.id);
        q.bindValue(":ticket", data.ticket);
        q.bindValue(":user", data.user);
        q.bindValue(":date", data.date);
        q.bindValue(":name", data.name);
        q.bindValue(":type", data.type);
        q.bindValue(":content", data.content);
        q.exec();
        return currval("ticket_files_id_seq");
        }

    Q_UNREACHABLE();
    return QVariant();
}


void DatabasePluginPostgres::remove(const Dbt::TicketFiles& id) {
    MSqlQuery q(m_db);
    q.prepare(R"'(delete from ticket_files where id = :id;)'");
    q.bindValue(":id", id.id);
    q.exec();
}


QList<Dbt::UsersCategories> DatabasePluginPostgres::usersCategories(int id, int user, const QString& category) {
    QList<Dbt::UsersCategories> list;
    MSqlQuery q(m_db);
    auto retvals = [&]() {
        q.exec();
        while (q.next()) {  
            int i=0; 
            Dbt::UsersCategories x; 
            x.id = q.value(i++).toInt();
            x.user = q.value(i++).toInt();
            x.category = q.value(i++).toString();
            list << x;
            }
        return list;
        };

    if (id > 0) {
        q.prepare(R"'(select id, "user", category from users_categories where id = :id)'");
        q.bindValue(":id", id);
        return retvals();
        }

    if (id <= 0 && category.isEmpty() && user > 0) {
        q.prepare(R"'(select id, "user", category from users_categories where "user" = :user)'");
        q.bindValue(":user", user);
        return retvals();
        }

    if (id <= 0 && category.isEmpty() && user <= 0) {
        q.prepare(R"'(select id, "user", category from users_categories)'");
        return retvals();
        }

    if (id <= 0 && !category.isEmpty() && user > 0) {
        q.prepare(R"'(select id, "user", category from users categories where "user" = :user and category = :category)'");
        q.bindValue(":user", user);
        q.bindValue(":category", category);
        return retvals();
        }

    if (id <= 0 && !category.isEmpty() && user <= 0) {
        q.prepare(R"'(select id, "user", category from users categories where category = :category)'");
        q.bindValue(":category", category);
        return retvals();
        }

    return list;
}


QList<Dbt::Overview> DatabasePluginPostgres::overview(const QString& category, const QStringList& statuses) {
    QString statusesX;
    QStringList statusesL;
    for (int i=0; i<statuses.size(); i++) {
        statusesL << "'" + statuses[i] + "'";
        }
    statusesL.sort();
    statusesX = "array[" +statusesL.join(",") + "]";

    QList<Dbt::Overview> list;
    MSqlQuery q(m_db);
    q.prepare(R"'(
        select key, category, statuses 
            from overview_params 
            where category = :category
              and statuses = )'" + statusesX );
    q.bindValue(":category", category);
    q.exec();
    if (q.next()) { 
        list = overview(q.value(0).toString());
        return list;
        }

    quint64 rnd = QRandomGenerator::global()->generate64();
    QString rnds = QString::number(rnd, 36).toUpper();
    q.prepare("insert into overview_params (key, category, statuses) values (:key, :category, " + statusesX + ");");
    q.bindValue(":key", rnds);
    q.bindValue(":category", category);
    q.exec();
    list = overview(rnds);
    return list;

}


QList<Dbt::Overview> DatabasePluginPostgres::overview(const QString& overviewId) {
    QList<Dbt::Overview> list;
    MSqlQuery q(m_db);
    Dbt::Overview overview;
    overview.id = overviewId.toUpper();

    q.prepare(R"'(
        select c.category, c.parent_category, c.description, c.price 
        from categories c, overview_params p
        where c.category = p.category
          and p.key = :key
        )'");
    q.bindValue(":key", overviewId.toUpper());
    q.exec();
    if (!q.next()) { return list; }
    overview.category.category          =      q.value(0).toString();
    overview.category.parent_category   = null(q.value(1).toString());
    overview.category.description       =      q.value(2).toString();
    overview.category.price             =      q.value(3).toDouble();

    q.exec(R"'(create temporary table overview_categories_tmp(category int);)'");
    q.exec(R"'(create temporary table overview_statuses_tmp(status text);)'");

    q.prepare(R"'(
            with recursive tree(category, parent_category) as (

                -- nerekurzivni
                select category, parent_category
                    from categories

                -- rekurzivni
                union all
                select category, parent_category
                    from tree t
                    where t.parent_category = category

            ),
            xcategory as (
                select category from overview_params where key = :key
            )

            insert into overview_categories_tmp 
            select category
            from tree
            where parent_category = (select category from xcategory)
            union all
            select category
            from categories
            where category = (select category from xcategory);
            )'");

    q.bindValue(":key", overviewId.toUpper());
    q.exec();

    q.prepare(R"'(insert into overview_statuses_tmp select unnest(statuses) from overview_params where key = :key;)'");
    q.bindValue(":key", overviewId.toUpper());
    q.exec();

    q.exec(R"'(
            with
            ticket_last_status as not materialized (
                select t.ticket, tl.status, st.description
                    from tickets t
                    left join lateral (select tn.ticket, tn.status
                                from  ticket_status tn, statuses s
                                where tn.ticket = t.ticket
                                  and tn.status = s.status
                                  and not s.ignored
                                order by ticket, date desc
                                limit 1
                                ) tl using (ticket)
                    left join statuses st on (st.status = tl.status)
                ),
            ticket_timesheets_sum as not materialized (
                select ticket, "user", sum(date_to - date_from) as duration
                    from ticket_timesheets
                    group by ticket, "user"
                )

            -- detaily
            select t.ticket, t.description, t."user", u.name, t.price as hour_price, to_hours(ts.duration), round(to_hours(ts.duration) * t.price), ls.description
                from tickets t
                left join ticket_last_status ls using (ticket)
                left join ticket_timesheets_sum ts using (ticket, "user")
                left join users u using ("user")
                where ls.status in (select status from overview_statuses_tmp)
                and category in (select category from overview_categories_tmp)

            union all
            -- Součet za jednotlivé tickety
            --     0         1              2     3     4     5           6        7
            select g.ticket, g.description, -1,   null, null, g.duration, g.price, st.description
                from (
                    select t.ticket, t.description, -1,   null, null, to_hours(sum(ts.duration)) as duration, sum(to_hours(ts.duration) * t.price) as price
                        from tickets t
                        left join ticket_timesheets_sum ts using (ticket, "user")
                        left join ticket_last_status ls using (ticket)
                        where ls.status in (select status from overview_statuses_tmp)
                        and category in (select category from overview_categories_tmp)
                        group by t.ticket
                    ) g
                left join ticket_last_status st using(ticket)

            union all
            -- Součet za jednotlivé statusy
            select null, null, -2, null, null, to_hours(sum(ts.duration)) as duration, round(sum(to_hours(ts.duration) * t.price)) as price, ls.description
                from ticket_last_status ls
                left join tickets t using (ticket)
                left join ticket_timesheets_sum ts using (ticket)
                where ls.status in (select status from overview_statuses_tmp)
                  and category in (select category from overview_categories_tmp)
                group by ls.description

            union all
            -- Součet za jednotlivé lidi
            --     0     1      2  3       4     5                                       6                                                     7
            select null, null, -3, u.name, null, to_hours(sum(ts.duration)) as duration, round(sum(to_hours(ts.duration) * t.price)) as price, null
                from ticket_last_status ls
                left join tickets t using (ticket)
                left join users u using ("user")
                left join ticket_timesheets_sum ts using (ticket)
                where ls.status in (select status from overview_statuses_tmp)
                  and category in (select category from overview_categories_tmp)
                group by t."user", u.name

            )'");
    while (q.next()) {
        if (q.value(2).toInt() == -1) {
            Dbt::Overview::TicketsSum s;
            s.ticket      = q.value(0).toInt();
            s.description = q.value(1).toString();
            s.duration    = q.value(5).toDouble();
            s.price       = q.value(6).toDouble();
            s.status      = q.value(7).toString();
            overview.ticketsSum << s;
            continue;
            }
        if (q.value(2).toInt() == -2) {
            Dbt::Overview::StatusSum s;
            s.duration    = q.value(5).toDouble();
            s.price       = q.value(6).toDouble();
            s.status      = q.value(7).toString();
            overview.statusSum << s;
            continue;
            }
        if (q.value(2).toInt() == -3) {
            Dbt::Overview::UserSum s;
            s.user_name   = q.value(3).toString();
            s.duration    = q.value(5).toDouble();
            s.price       = q.value(6).toDouble();
            overview.userSum << s;
            continue;
            }

        int i = 0;
        Dbt::Overview::Tickets t;
        t.ticket        = q.value(i++).toInt();
        t.description   = q.value(i++).toString();
        t.user          = q.value(i++).toInt();
        t.user_name     = q.value(i++).toString();
        t.hour_price    = q.value(i++).toDouble();
        t.duration      = q.value(i++).toDouble();
        t.price         = q.value(i++).toDouble();
        t.status        = q.value(i++).toString();
        overview.tickets << t;
        }

    q.exec(R"'(
            with
            ticket_last_status as not materialized (
                select t.ticket, tl.status
                    from tickets t
                    left join lateral (select tn.ticket, tn.status
                                from  ticket_status tn, statuses s
                                where tn.ticket = t.ticket
                                  and tn.status = s.status
                                  and not s.ignored
                                order by ticket, date desc
                                limit 1
                                ) tl using (ticket)
                ),
            ticket_timesheets_sum as not materialized (
                select ticket, "user", date_to::date as date, sum(date_to - date_from) as duration
                    from ticket_timesheets
                    group by ticket, "user", date_to::date
                )

            select t.ticket, t.description, t."user", u.name, t.price as hour_price, ts.date, to_hours(ts.duration), round(to_hours(ts.duration) * t.price)
                from tickets t
                left join users u using ("user")
                left join ticket_last_status ls using (ticket)
                left join ticket_timesheets_sum ts using (ticket, "user")
                left join categories c using (category)
                where ls.status in (select status from overview_statuses_tmp)
                and category in (select category from overview_categories_tmp)

            union all
            --     0     1     2   3     4     5     6              7
            select null, null, -1, null, null, null, to_hours(sum(duration)), sum(round(to_hours(ts.duration) * t.price))
                from tickets t
                left join ticket_last_status ls using (ticket)
                left join ticket_timesheets_sum ts using (ticket, "user")
                where ls.status in (select status from overview_statuses_tmp)
                and category in (select category from overview_categories_tmp)
            order by date
            )'");
    while (q.next()) {
        if (q.value(2).toInt() == -1) {
            overview.sum.duration    = q.value(6).toDouble();
            overview.sum.price       = q.value(7).toDouble();
            continue;
            }
        int i = 0;
        Dbt::Overview::Days t;
        t.ticket        = q.value(i++).toInt();
        t.description   = q.value(i++).toString();
        t.user          = q.value(i++).toInt();
        t.user_name     = q.value(i++).toString();
        t.hour_price    = q.value(i++).toDouble();
        t.date          = q.value(i++).toDateTime();
        t.duration      = q.value(i++).toDouble();
        t.price         = q.value(i++).toDouble();
        overview.days << t;
        }

    list << overview;
    return list;
}


