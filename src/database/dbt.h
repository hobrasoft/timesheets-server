/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _DBT_H_
#define _DBT_H_

#include <QString>
#include <QDateTime>
#include <QCoreApplication>
#include <QVariantList>
#include <QVariant>
#include "version.h"


template<typename T>
QVariantList toList(const QList<T>& input) {
    QVariantList list;
    QListIterator<T> iterator(input);
    while (iterator.hasNext()) {
        list << iterator.next().toMap();
        }
    return list;
}


namespace Dbt {

struct Users {
    int         user;
    QString     login;
    QString     name;
    QString     lang;
    bool        enabled;
    bool        admin;
    //
    Users() { user = 0; enabled = false; admin = false; }
    Users(int id) { user = id; enabled = false; admin = false; }
    QVariantMap toMap() const;
    static Users fromMap(const QVariantMap&);
};


struct Categories {
    QString     category;           // primary key
    QString     parent_category;
    QString     description;
    double      price;

    QVariantMap toMap() const;
    static Categories fromMap(const QVariantMap&);
    Categories() { price = 0; }
    Categories(const QString& id) { price = 0; category = id; }
};


struct UsersCategories {
    int         id;
    QString     category;
    int         user;

    QVariantMap toMap() const;
    static UsersCategories fromMap(const QVariantMap&);
    UsersCategories() { id = -1; user = -1; }
    UsersCategories(int i) { id = i; }
};


struct StatusOrder {
    QVariant    id;
    QVariant    category;
    QVariant    previous_status;
    QVariant    next_status;

    QVariantMap toMap() const;
    static StatusOrder fromMap(const QVariantMap&);
    StatusOrder(const QString& pid) { id = pid; }
    StatusOrder() {}
};


struct Statuses {
    QString     status;
    QString     description;
    QString     abbreviation;
    QString     color;
    bool        closed;

    QVariantMap toMap() const;
    static Statuses fromMap(const QVariantMap&);
    Statuses(const QString& id) { status = id; closed = false; }
    Statuses() { closed = false; }
};


// Tabulky modifikovatelné
struct Mutable {
    int         id;
    QVariant    user;
    QVariant    ticket;
    bool        modified;
    bool        created;
    //
    virtual QVariantMap toMap() const;
    static Mutable fromMap(const QVariantMap&);
    Mutable() { id = 0; modified = false; created = false; }
    Mutable(const QVariant& x) { id = x.toInt(); modified = false; created = false; }
    Mutable(const Mutable& x) { operator=(x); }
    Mutable& operator=(const Mutable& x) {
        id = x.id;
        user = x.user;
        ticket = x.ticket;
        modified = x.modified;
        created = x.created;
        return *this;
        }
};


struct TicketStatus : Mutable {
    QDateTime   date;
    QString     status;
    QString     description;
    //
    QVariantMap toMap() const Q_DECL_OVERRIDE;
    static TicketStatus fromMap(const QVariantMap&);
    static QList<TicketStatus> fromList(const QVariantList&);
    TicketStatus() : Mutable() {}
    TicketStatus(const QVariant& x) : Mutable(x) {}
    TicketStatus(const Mutable& x) : Mutable(x) {}
    TicketStatus(const TicketStatus& x) : Mutable() { operator=(x); }
    TicketStatus& operator=(const TicketStatus& x) {
        Mutable::operator=(x);
        date = x.date;
        status = x.status;
        description = x.description;
        return *this;
        }
};


struct TicketTimesheets : Mutable {
    QDateTime   date_from;
    QDateTime   date_to;
    //
    QVariantMap toMap() const Q_DECL_OVERRIDE;
    static TicketTimesheets fromMap(const QVariantMap&);
    static QList<TicketTimesheets> fromList(const QVariantList&);
    TicketTimesheets() : Mutable() {}
    TicketTimesheets(const QVariant& x) : Mutable(x) {}
    TicketTimesheets(const Mutable& x) : Mutable(x) {}
    TicketTimesheets(const TicketTimesheets& x) : Mutable() { operator=(x); }
    TicketTimesheets& operator=(const TicketTimesheets& x) {
        Mutable::operator=(x);
        date_from = x.date_from;
        date_to = x.date_to;
        return *this;
        }

};


struct TicketFiles : Mutable {
    QDateTime   date;
    QString     name;
    QString     type;
    QByteArray  content;

    QVariantMap toMap() const Q_DECL_OVERRIDE;
    static TicketFiles fromMap(const QVariantMap&);
    static QList<TicketFiles> fromList(const QVariantList&);
    TicketFiles() : Mutable() {}
    TicketFiles(const QVariant& x) : Mutable(x) {}
    TicketFiles(const Mutable& x) : Mutable(x) {}
    TicketFiles(const TicketFiles& x) : Mutable() { operator=(x); }
    TicketFiles& operator=(const TicketFiles& x) {
        Mutable::operator=(x);
        date = x.date;
        name = x.name;
        type = x.type;
        content = x.content;
        return *this;
        }
};


struct TicketValues : Mutable {
    QDateTime   date;
    QString     name;
    QVariant    value;

    QVariantMap toMap() const Q_DECL_OVERRIDE;
    static TicketValues fromMap(const QVariantMap&);
    static QList<TicketValues> fromList(const QVariantList&);
    TicketValues() : Mutable() {}
    TicketValues(const QVariant& x) : Mutable(x) {}
    TicketValues(const Mutable& x) : Mutable(x) {}
    TicketValues(const TicketValues& x) : Mutable() { operator=(x); }
    TicketValues& operator=(const TicketValues& x) {
        Mutable::operator=(x);
        date = x.date;
        name = x.name;
        value = x.value;
        return *this;
        }

};


struct Tickets {
    QVariant    ticket;
    QVariant    category;
    QDateTime   date;
    QVariant    price;
    QString     description;
    int         user;

    virtual QVariantMap toMap() const;
    static Tickets fromMap(const QVariantMap&);
    Tickets() { user = 0; }
    Tickets(const QVariant& x) { ticket = x; }
    Tickets(const Tickets& x) { operator=(x); }
    Tickets& operator=(const Tickets& x) {
        ticket = x.ticket;
        category = x.category;
        date = x.date;
        price = x.price;
        description = x.description ;
        user = x.user;
        return *this;
        }

};


struct TicketsVw : Tickets {
    QList<Dbt::TicketTimesheets> timesheets;
    QList<Dbt::TicketStatus>     statuses;
    QList<Dbt::TicketFiles>      files;
    QList<Dbt::TicketValues>     values;

    QVariantMap toMap() const Q_DECL_OVERRIDE;
    static TicketsVw fromMap(const QVariantMap&);
    TicketsVw() : Tickets() {}
    TicketsVw(const QVariant& x) : Tickets(x) { }
    TicketsVw(const Tickets& x) : Tickets(x) {}
    TicketsVw(const TicketsVw& x) : Tickets() { operator=(x); }
    TicketsVw& operator=(const TicketsVw& x) {
        Tickets::operator=(x);
        timesheets = x.timesheets;
        statuses = x.statuses;
        files = x.files;
        values = x.values;
        return *this;
        }

};


}

#endif

