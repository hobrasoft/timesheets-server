/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "database_plugin_fotomon.h"
#include "msettings.h"
#include "msqlquery.h"
#include "pdebug.h"
#include <QUuid>
#include <QSqlError>
#include <QCryptographicHash>

using namespace Db::Plugins;


DatabasePluginFotomon::~DatabasePluginFotomon() {
    close();
}


DatabasePluginFotomon::DatabasePluginFotomon(QObject *parent) : Db::Plugins::DatabasePlugin(parent) {
    Q_ASSERT(parent != NULL);
    setObjectName("DatabasePluginFotomon");
}


bool DatabasePluginFotomon::open() {
    m_db = QSqlDatabase::addDatabase("QPSQL", QUuid::createUuid().toString().toUtf8());

    m_db.setDatabaseName ( m_databasename );
    m_db.setHostName     ( m_hostname     );
    m_db.setPort         ( m_port         );
    m_db.setUserName     ( m_username     );
    m_db.setPassword     ( m_password     );

    if (!m_db.open()) {
        PDEBUG << "Cannot connect to database Fotomon";
        PDEBUG << m_db.lastError().text();
        return false;
        }

    return true;
}


bool DatabasePluginFotomon::close() {
    m_db.close();
    return true;
}


void DatabasePluginFotomon::begin() {
    MSqlQuery q(m_db);
    q.exec("begin;");
}


void DatabasePluginFotomon::commit() {
    MSqlQuery q(m_db);
    q.exec("commit;");
}

  
QList<Dbt::Users> DatabasePluginFotomon::authenticate(const QString& login, const QString& password) {
    QString md5 = QString::fromUtf8(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex());
    QList<Dbt::Users> list;
    MSqlQuery q(m_db);
    q.prepare("select \"user\", login, name, language from users where login = :login and password = :password and is_active and not is_deleted;");
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


QList<Dbt::Users> DatabasePluginFotomon::users(int id) {
    QList<Dbt::Users> list;
    return list;
}


void DatabasePluginFotomon::createCategoriesTemporaryTable() {
    MSqlQuery q(m_db);

    q.exec("create temporary table timesheet_categories "
            "(type int, system int, category int, description text, parent_type int)"
            ";");

    // Vybere všechny typy knih
    q.prepare("insert into timesheet_categories (type, system, category, description, parent_type) "
            " select type, null, null, formal_description->>:lang, 1 "
            " from tickets_types;");
    q.bindValue(":lang", userLang());   
    q.exec();

    // Vybere všechny kombinace záznamů typ-system
    q.prepare("insert into timesheet_categories (type, system, category, description, parent_type) "
            " select distinct on (tt.type, s.system) tt.type, s.system, null, s.description, 2 "
            " from tickets_categories_types_systems tt, systems s "
            " where s.show_on_panel and s.show_on_web "
            " and s.system in (select system from users_systems where \"user\" = :user); "
            );
    q.bindValue(":user", userId());
    q.exec();

    // Vybere všechny kombinace záznamů typ-system-kategorie, kde EXISTUJE záznam typ-system
    q.prepare("insert into timesheet_categories (type, system, category, description, parent_type) "
            " select tt.type, s.system, tc.category, tc.formal_description->>:lang, 3 "
            " from tickets_categories_types_systems tt, systems s, tickets_categories tc "
            " where tt.system = s.system "
            " and tc.category = tt.category "
            " and s.show_on_panel and s.show_on_web "
            " and s.system in (select system from users_systems where \"user\" = :user); "
            );
    q.bindValue(":lang", userLang());
    q.bindValue(":user", userId());
    q.exec();

    // Vybere všechny kombinace záznamů typ-system-kategorie, kde NEEXISTUJE záznam typ-system
    q.prepare("insert into timesheet_categories (type, system, category, description, parent_type) "
            " select tt.type, s.system, tc.category, tc.formal_description->>:lang, 4 "
            " from systems s, tickets_types tt, tickets_categories tc "
            " where s.show_on_panel and s.show_on_web "
            " and s.system in (select system from users_systems where \"user\" = :user) "
            " and not exists (select 1 "
            "       from tickets_categories_types_systems tcts "
            "      where tcts.type = tt.type "
            "        and tcts.system = s.system ); "
            );
    q.bindValue(":user", userId());
    q.bindValue(":lang", userLang());
    q.exec();

}


QString DatabasePluginFotomon::categoryKey(const QVariant& type, const QVariant& system, const QVariant& category, int parent_type) {
    if (parent_type == 1) { 
        return QString(R"'({"type":%1})'")
            .arg(type.toInt())
            ;
        }

    if (parent_type == 2) {
        return QString(R"'({"type":%1,"system":%2})'")
            .arg(type.toInt())
            .arg(system.toInt())
            ;
        }

    if (parent_type == 3) {
        return QString(R"'({"type":%1,"system":%2,"category":%3})'")
            .arg(type.toInt())
            .arg(system.toInt())
            .arg(category.toInt())
            ;
        }

    if (parent_type == 4) {
        return QString(R"'({"type":%1,"system":%2,"category":%3})'")
            .arg(type.toInt())
            .arg(system.toInt())
            .arg(category.toInt())
            ;
        }

    return QString();
}


QString DatabasePluginFotomon::parentCategoryKey(const QVariant& type, const QVariant& system, const QVariant& category, int parent_type) {
    Q_UNUSED(category);
    if (parent_type == 1) { 
        return QString();
        }

    if (parent_type == 2) {
        return QString(R"'({"type":%1})'")
            .arg(type.toInt())
            ;
        }

    if (parent_type == 3) {
        return QString(R"'({"type":%1,"system":%2})'")
            .arg(type.toInt())
            .arg(system.toInt())
            ;
        }

    if (parent_type == 4) {
        return QString(R"'({"type":%1,"system":%2})'")
            .arg(type.toInt())
            .arg(system.toInt())
            ;
        }

    return QString();
}


QList<Dbt::Categories> DatabasePluginFotomon::categories(const QString& id) {
    createCategoriesTemporaryTable();

    QList<Dbt::Categories> list;
    MSqlQuery q(m_db);

    q.exec("select type, system, category, description, parent_type from timesheet_categories;");
    while (q.next()) {
        Dbt::Categories x;
        QVariant type = q.value(0);
        QVariant system = q.value(1);
        QVariant category = q.value(2);
        QVariant description = q.value(3);
        int parent_type = q.value(4).toInt();

        x.category          =       categoryKey(type, system, category, parent_type);
        x.parent_category   = parentCategoryKey(type, system, category, parent_type);
        x.description       = description.toString();
        list << x;
        }

    PDEBUG << "pocet kategorii" << list.size();
    return list;
}


QList<Dbt::StatusOrder> DatabasePluginFotomon::statusOrder(const QString& id) {
    createCategoriesTemporaryTable();

    QList<Dbt::StatusOrder> list;
    MSqlQuery q(m_db);

    q.exec("select c.type, c.system, c.category, c.description, c.parent_type, "
        " s.status_from, s.status_to "
        " from timesheet_categories c, tickets_types_status s"
        " where c.type = s.type "
        ";");
    while (q.next()) {
        Dbt::StatusOrder x;
        QVariant type = q.value(0);
        QVariant system = q.value(1);
        QVariant category = q.value(2);
        QVariant description = q.value(3);
        int parent_type = q.value(4).toInt();

        x.previous_status   = q.value(5);
        x.next_status       = q.value(6);
        x.category          = categoryKey(type, system, category, parent_type);
        list << x;
        }

    PDEBUG << "pocet prechodu" << list.size();
    return list;

}


QList<Dbt::Statuses> DatabasePluginFotomon::statuses(const QString& id) {
    QList<Dbt::Statuses> list;
    MSqlQuery q(m_db);

    q.prepare("select ts.status, ts.formal_description->>:lang, ts.abbreviation, ts.color "
        " from tickets_status ts "
        ";");
    q.bindValue(":lang", userLang());   
    q.exec();
    while (q.next()) {
        Dbt::Statuses x;
        int i=0;
        x.status = q.value(i++).toString();
        x.description = q.value(i++).toString();
        x.abbreviation = q.value(i++).toString();
        x.color = q.value(i++).toString();
        list << x;
        }

    PDEBUG << "pocet statusu" << list.size();
    return list;
}


QList<Dbt::Tickets> DatabasePluginFotomon::tickets(bool all) {
    return tickets(-1, all);
}


QList<Dbt::Tickets> DatabasePluginFotomon::tickets(int ticket, bool all) {
    PDEBUG;
    QList<Dbt::Tickets> list;
    MSqlQuery q(m_db);

    q.prepare(R"'(
        with
        ending_status as (
            select distinct  t1.type, t1.status_to as status from tickets_types_status t1, tickets_status ts  where t1.status_to = ts.status and ts.closed
            ),
        tickets_last_status as (
            select t.ticket, t.type, tl.status
                from tickets t
                left join lateral (select tn.ticket, tn.status from tickets_notes tn where tn.ticket = t.ticket order by ticket, date desc limit 1) tl using (ticket)
            ),
        closed_tickets as (
            select distinct ts.ticket from tickets_last_status ts, ending_status es where ts.status = es.status and ts.type = es.type
            ),
        active_tickets as (
            select t1.ticket from tickets t1 where t1.ticket not in (select ticket from closed_tickets)
            ),
        users_systems as (
            select us.system 
                from users_systems us, users u, systems s
                where u."user" = :user 
                  and u."user" = us."user"
                  and us.system = s.system
                  and s.show_on_web and s.show_on_panel
                  and u.is_active and not u.is_deleted
            )

        select ticket, type, system, category, date, case when description = '' then formal_description->0->'description'->>:lang else description end as description
            from tickets
            where ticket in (select ticket from active_tickets)
              and system in (select system from users_systems)
              and (:ticket1 <= 0 or :ticket1 = ticket)
            ;
        )'");
    q.bindValue(":user", userId());
    q.bindValue(":lang", userLang());   
    q.bindValue(":ticket1", ticket);
    q.bindValue(":ticket2", ticket);
    q.exec();
    while (q.next()) {
        Dbt::Tickets x;
        int i=0;
        x.ticket = q.value(i++);
        QVariant type = q.value(i++);
        QVariant system = q.value(i++);
        QVariant category = q.value(i++);
        x.category = categoryKey(type, system, category, 3);
        x.date = q.value(i++).toDateTime();
        x.description = q.value(i++).toString();
        list << x;
        }
    return list;
}


QList<Dbt::TicketStatus> DatabasePluginFotomon::ticketStatus(int ticket, bool all) {
    PDEBUG;
    QList<Dbt::TicketStatus> list;
    MSqlQuery q(m_db);

    q.prepare(R"'(
        with
        ending_status as (
            select distinct  t1.type, t1.status_to as status from tickets_types_status t1, tickets_status ts  where t1.status_to = ts.status and ts.closed
            ),
        tickets_last_status as (
            select t.ticket, t.type, tl.status
                from tickets t
                left join lateral (select tn.ticket, tn.status from tickets_notes tn where tn.ticket = t.ticket order by ticket, date desc limit 1) tl using (ticket)
            ),
        closed_tickets as (
            select distinct ts.ticket from tickets_last_status ts, ending_status es where ts.status = es.status and ts.type = es.type
            ),
        active_tickets as (
            select t1.ticket from tickets t1 where t1.ticket not in (select ticket from closed_tickets)
            ),
        users_systems as (
            select us.system 
                from users_systems us, users u, systems s
                where u."user" = :user 
                  and u."user" = us."user"
                  and us.system = s.system
                  and s.show_on_web and s.show_on_panel
                  and u.is_active and not u.is_deleted
            )

        select tn.note, tn.ticket, t."user", tn.date, tn.status,
                case when tn.description != '' then tn.description else tn.formal_description->0->'description'->>'cs' end as description
            from tickets_notes tn
            left join tickets t on (t.ticket = tn.ticket)
            left join users u on (tn."user" = u."user")
            where t.ticket in (select ticket from active_tickets)
              and t.system in (select system from users_systems)
              and (:ticket1 <= 0 or :ticket1 = ticket)
            ;
        )'");
    q.bindValue(":user", userId());
    q.bindValue(":lang", userLang());   
    q.bindValue(":ticket1", ticket);
    q.bindValue(":ticket2", ticket);
    q.exec();
    while (q.next()) {
        Dbt::TicketStatus x;
        int i=0;
        x.id            = q.value(i++);
        x.ticket        = q.value(i++);
        x.user          = q.value(i++);
        x.date          = q.value(i++).toDateTime();
        x.status        = q.value(i++);
        x.description   = q.value(i++).toString();
        list << x;
        }
    return list;
}


QList<Dbt::TicketStatus> DatabasePluginFotomon::ticketStatus(bool all) {
    return ticketStatus(-1, all);
}


QList<Dbt::TicketStatus> DatabasePluginFotomon::ticketStatus(int id) {
    QList<Dbt::TicketStatus> list;
    return list;
}


QList<Dbt::TicketsVw> DatabasePluginFotomon::ticketsVw(bool all) {
    return ticketsVw(-1, all);
}

QList<Dbt::TicketsVw> DatabasePluginFotomon::ticketsVw(int ticket, bool all) {
    QList<Dbt::TicketsVw> list;
    return list;
}


QList<Dbt::TicketTimesheets> DatabasePluginFotomon::ticketTimesheets(int ticket, bool all) {
    QList<Dbt::TicketTimesheets> list;
    return list;
}


QList<Dbt::TicketTimesheets> DatabasePluginFotomon::ticketTimesheets(bool all) {
    QList<Dbt::TicketTimesheets> list;
    return list;
}


QList<Dbt::TicketTimesheets> DatabasePluginFotomon::ticketTimesheets(int id) {
    QList<Dbt::TicketTimesheets> list;
    return list;
}


QList<Dbt::TicketFiles> DatabasePluginFotomon::ticketFiles(int ticket, bool all) {
    QList<Dbt::TicketFiles> list;
    return list;
}


QList<Dbt::TicketFiles> DatabasePluginFotomon::ticketFiles(bool all) {
    QList<Dbt::TicketFiles> list;
    return list;
}


QList<Dbt::TicketFiles> DatabasePluginFotomon::ticketFiles(int ticket) {
    QList<Dbt::TicketFiles> list;
    return list;
}


QList<Dbt::TicketValues> DatabasePluginFotomon::ticketValues(int ticket, bool all) {
    QList<Dbt::TicketValues> list;
    return list;
}


QList<Dbt::TicketValues> DatabasePluginFotomon::ticketValues(bool all) {
    QList<Dbt::TicketValues> list;
    return list;
}


QList<Dbt::TicketValues> DatabasePluginFotomon::ticketValues(int ticket) {
    QList<Dbt::TicketValues> list;
    return list;
}



