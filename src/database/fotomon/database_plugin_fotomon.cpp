/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "database_plugin_fotomon.h"
#include "msettings.h"
#include "msqlquery.h"
#include "json.h"
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
    m_temporaryTableTicketsCreated = false;
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


QVariant DatabasePluginFotomon::currval(const QString& sequence) {
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

  
QList<Dbt::Users> DatabasePluginFotomon::authenticate(const QString& login, const QString& password) {
    QString md5 = QString::fromUtf8(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex());
    QList<Dbt::Users> list;
    MSqlQuery q(m_db);
    q.prepare(R"X(
        select "user", login, name, language
            from users
            where password = :password
              and login = :login
              and is_active
              and not is_deleted
            ;
        )X");
    q.bindValue(":login", login);
    q.bindValue(":password", md5);
    q.exec();
    while (q.next()) {
        int i=0;
        Dbt::Users x;
        x.user  = q.value(i++).toInt();
        x.login = q.value(i++).toString();
        x.name  = q.value(i++).toString();
        x.lang  = q.value(i++).toString();
        list << x;
        }
    return list;
}


QList<Dbt::Users> DatabasePluginFotomon::users(int id) {
    Q_UNUSED(id);
    QList<Dbt::Users> list;
    MSqlQuery q(m_db);
    q.prepare(R"X(select "user", login, name, language from users where is_active and not is_deleted;)X");
    q.exec();
    while (q.next()) {
        int i=0;
        Dbt::Users x;
        x.user  = q.value(i++).toInt();
        x.login = q.value(i++).toString();
        x.name  = q.value(i++).toString();
        x.lang  = q.value(i++).toString();
        list << x;
        }
    return list;
}


void DatabasePluginFotomon::createTemporaryTableTickets(int ticket, bool all) {
    Q_UNUSED(all);
    if (m_temporaryTableTicketsCreated) { return; }
    m_temporaryTableTicketsCreated = true;
    MSqlQuery q(m_db);
    q.exec(R"'(
        create temporary table temporary_tickets (
            ticket      int,
            type        int,
            system      int,
            category    int,
            date        timestamp with time zone,
            "user"      int,
            description text
            );
        )'");

    q.prepare(R"'(
        with
        active_tickets as (
            select t.ticket
                from tickets t
                left join lateral (select tn.ticket, tn.status from tickets_notes tn where tn.ticket = t.ticket order by ticket, date desc limit 1) tl using (ticket)
                left join tickets_status ts on (tl.status = ts.status)
                where (:ticket1 <= 0 or :ticket2 = t.ticket)
                  and (ts.closed is null or not ts.closed)
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
        insert into temporary_tickets (ticket, type, system, category, date, "user", description)
        select t.ticket, t.type, t.system, t.category, t.date, t."user", 
            case when description = '' then formal_description->0->'description'->>:lang else description end as description
        from tickets t, users u
        where u."user" = t."user"
          and t.ticket in (select ticket from active_tickets)
          and t.system in (select system from users_systems)
              and (:ticket3 <= 0 or :ticket4 = t.ticket)
        )'");
    q.bindValue(":user", userId());
    q.bindValue(":lang", userLang());
    q.bindValue(":ticket1", ticket);
    q.bindValue(":ticket2", ticket);
    q.bindValue(":ticket3", ticket);
    q.bindValue(":ticket4", ticket);
    q.exec();
}


void DatabasePluginFotomon::createTemporaryTableCategories() {
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
    Q_UNUSED(id);
    createTemporaryTableCategories();

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


QList<Dbt::Categories> DatabasePluginFotomon::subcategories(const QString& id) {
    QList<Dbt::Categories> list;
    return list;
}


QList<Dbt::StatusOrder> DatabasePluginFotomon::statusOrder(const QString& id) {
    Q_UNUSED(id);
    createTemporaryTableCategories();

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
    Q_UNUSED(id);
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

    return list;
}


QList<Dbt::Tickets> DatabasePluginFotomon::tickets(bool all) {
    return tickets(-1, all);
}


QList<Dbt::Tickets> DatabasePluginFotomon::tickets(int ticket, bool all) {
    PDEBUG;
    createTemporaryTableTickets(ticket, all);
    QList<Dbt::Tickets> list;
    MSqlQuery q(m_db);

    q.prepare(R"X(select ticket, type, system, category, date, description from temporary_tickets;)X");
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


QList<Dbt::TicketsVw> DatabasePluginFotomon::ticketsVw(const Dbt::Categories&) {
    return ticketsVw(-1, true);
}


QList<Dbt::TicketsVw> DatabasePluginFotomon::ticketsVw(bool all) {
    Q_UNUSED(all);
    return ticketsVw(-1, all);
}


QList<Dbt::TicketsVw> DatabasePluginFotomon::ticketsVw(int ticket, bool all) {
    createTemporaryTableTickets(ticket, all);
    QList<Dbt::Tickets> list1;
    QList<Dbt::TicketsVw> list;
    MSqlQuery q(m_db);

    q.prepare(R"X(select ticket, type, system, category, date, description, "user" from temporary_tickets;)X");
    q.exec();
    while (q.next()) {
        Dbt::TicketsVw x;
        int i=0;
        x.ticket = q.value(i++);
        QVariant type = q.value(i++);
        QVariant system = q.value(i++);
        QVariant category = q.value(i++);
        x.category = categoryKey(type, system, category, 3);
        x.date = q.value(i++).toDateTime();
        x.description = q.value(i++).toString();
        x.user = q.value(i++).toInt();
        list1 << x;
        }
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


QVariant DatabasePluginFotomon::save(const Dbt::Tickets& data) {
    MSqlQuery q(m_db);

    QVariantMap x = JSON::data(data.category.toByteArray()).toMap();
    int type = x["type"].toInt();
    int system = x["system"].toInt();
    int category = x["category"].toInt();

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
                type = :type,
                system = :system,
                category = :category,
                date = :date,
                price = :price,
                description = :description,
               "user" = :user
                where ticket = :ticket
            )'");
        q.bindValue(":type", type);
        q.bindValue(":system", system);
        q.bindValue(":category", category);
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
            insert into tickets (type, system, category, date, price, description, "user")
                        values (:type, :system, :category, :date, :price, :description, :user);
            )'");
        q.bindValue(":type", type);
        q.bindValue(":system", system);
        q.bindValue(":category", category);
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


QVariant DatabasePluginFotomon::save(const Dbt::TicketsVw& data) {
    MSqlQuery q(m_db);
    q.begin();
    int ticket = save(dynamic_cast<const Dbt::Tickets&>(data)).toInt();
    save(remapTicket(data.timesheets, ticket));
    save(remapTicket(data.statuses, ticket));
    save(remapTicket(data.values, ticket));
    save(remapTicket(data.files, ticket));
    q.commit();

    return QVariant();
}


void DatabasePluginFotomon::remove(const Dbt::Tickets& id) {
    MSqlQuery q(m_db);
    q.prepare(R"'(delete from tickets where ticket = :id;)'");
    q.bindValue(":id", id.ticket);
    q.exec();
}


QList<Dbt::TicketStatus> DatabasePluginFotomon::ticketStatus(int ticket, bool all) {
    createTemporaryTableTickets(ticket, all);
    QList<Dbt::TicketStatus> list;
    MSqlQuery q(m_db);

    q.prepare(R"'(
        select tn.note, tn.ticket, t."user", tn.date, tn.status,
                case when tn.description != '' then tn.description else tn.formal_description->0->'description'->>'cs' end as description
            from tickets_notes tn, temporary_tickets t
            where tn.ticket = t.ticket
              and (:ticket1 <= 0 or :ticket2 = t.ticket)
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
        x.id            = q.value(i++).toInt();
        x.ticket        = q.value(i++).toInt();
        x.user          = q.value(i++).toInt();
        x.date          = q.value(i++).toDateTime();
        x.status        = q.value(i++).toString();
        x.description   = q.value(i++).toString();
        list << x;
        }
    return list;
}


QList<Dbt::TicketStatus> DatabasePluginFotomon::ticketStatus(bool all) {
    Q_UNUSED(all);
    return ticketStatus(-1, all);
}


QList<Dbt::TicketStatus> DatabasePluginFotomon::ticketStatus(int id) {
    Q_UNUSED(id);
    QList<Dbt::TicketStatus> list;
    return list;
}


QVariant DatabasePluginFotomon::save(const Dbt::TicketStatus& data) {
    MSqlQuery q(m_db);

    bool found = false;
    if (!data.created) {
        q.prepare(R"'(select 1 from tickets_notes where note = :id;)'");
        q.bindValue(":id", data.id);
        q.exec();
        found = q.next();
        }

    if (!data.created && found) {
        q.prepare(R"'(
            update tickets_notes set
                ticket = :ticket,
               "user" = :user,
                date = :date,
                description = :description,
                status = :status
                where note = :id
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
            insert into tickets_notes (ticket, "user", date, description, status)
                values (:ticket, :user, :date, :description, :status)
            )'");
        q.bindValue(":user", data.user);
        q.bindValue(":ticket", data.ticket);
        q.bindValue(":date", data.date);
        q.bindValue(":description", data.description);
        q.bindValue(":status", data.status);
        q.exec();

        return currval("tickets_notes_note_seq");
        }

    Q_UNREACHABLE();
    return QVariant();
}


QList<Dbt::TicketTimesheets> DatabasePluginFotomon::ticketTimesheets(int ticket, bool all) {
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


QList<Dbt::TicketTimesheets> DatabasePluginFotomon::ticketTimesheets(int id) {
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


QList<Dbt::TicketTimesheets> DatabasePluginFotomon::ticketTimesheets(bool all) {
    return ticketTimesheets(-1, all);
}


QVariant DatabasePluginFotomon::save(const Dbt::TicketTimesheets& data) {
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


void DatabasePluginFotomon::remove(const Dbt::TicketTimesheets& id) {
    MSqlQuery q(m_db);
    q.prepare(R"'(delete from ticket_timesheets where id = :id;)'");
    q.bindValue(":id", id.id);
    q.exec();
}



QList<Dbt::TicketFiles> DatabasePluginFotomon::ticketFiles(int ticket, bool all) {
    createTemporaryTableTickets(ticket, all);
    QList<Dbt::TicketFiles> list;
    MSqlQuery q(m_db);

    q.prepare(R"'(
        select f.id, f."user", f.date, f.ticket, f.name, f.type, f.content
            from temporary_tickets t, ticket_images f
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


QList<Dbt::TicketFiles> DatabasePluginFotomon::ticketFiles(int id) {
    QList<Dbt::TicketFiles> list;
    MSqlQuery q(m_db);

    q.prepare(R"'(
        select f.id, f."user", f.date, f.ticket, f.name, f.type, f.content
            from ticket_images f, users u, tickets t, users_categories uc
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


QList<Dbt::TicketFiles> DatabasePluginFotomon::ticketFiles(bool all) {
    return ticketFiles(-1, all);
}


QVariant DatabasePluginFotomon::save(const Dbt::TicketFiles& data) {
    MSqlQuery q(m_db);

    bool found = false;
    if (!data.created) {
        q.prepare(R"'(select 1 from ticket_images where id = :id;)'");
        q.bindValue(":id", data.id);
        q.exec();
        found = q.next();
        }

    if (!data.created) {
        q.prepare(R"'(
            update ticket_images set
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
            insert into ticket_images (ticket, "user", date, name, type, content)
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
        return currval("ticket_images_id_seq");
        }

    Q_UNREACHABLE();
    return QVariant();
}


void DatabasePluginFotomon::remove(const Dbt::TicketFiles& id) {
    MSqlQuery q(m_db);
    q.prepare(R"'(delete from ticket_images where id = :id;)'");
    q.bindValue(":id", id.id);
    q.exec();
}


QList<Dbt::TicketValues> DatabasePluginFotomon::ticketValues(int ticket, bool all) {
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


QList<Dbt::TicketValues> DatabasePluginFotomon::ticketValues(int id) {
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


QList<Dbt::TicketValues> DatabasePluginFotomon::ticketValues(bool all) {
    return ticketValues(-1, all);
}


QVariant DatabasePluginFotomon::save(const Dbt::TicketValues& data) {
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
    return QVariant();
}


void DatabasePluginFotomon::remove(const Dbt::TicketValues& id) {
    MSqlQuery q(m_db);
    q.prepare(R"'(delete from ticket_values where id = :id;)'");
    q.bindValue(":id", id.id);
    q.exec();
}


QList<Dbt::UsersCategories> DatabasePluginFotomon::usersCategories(int id, int user, const QString& category) {
    Q_UNUSED(id);
    Q_UNUSED(user);
    Q_UNUSED(category);
    QList<Dbt::UsersCategories> list;
    return list;
}

