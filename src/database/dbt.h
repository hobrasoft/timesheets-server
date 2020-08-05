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


struct TicketFiles {
    QVariant    id;
    QVariant    ticket;
    QVariant    user;
    QString     name;
    QString     type;
    QByteArray  content;

    TicketFiles(const QVariant& x) { id = x; }
    TicketFiles(){}
    QVariantMap toMap() const;
    static TicketFiles fromMap(const QVariantMap&);
};


struct TicketValues {
    QVariant    id;
    QVariant    ticket;
    QVariant    user;
    QString     name;
    QString     value;

    TicketValues(const QVariant& x) { id = x; }
    TicketValues() {}
    QVariantMap toMap() const;
    static TicketValues fromMap(const QVariantMap&);
};


struct TicketStatus {
    QVariant    id;
    QVariant    ticket;
    QVariant    user;
    QDateTime   date;   
    QString     description;
    QVariant    status;

    TicketStatus(const QVariant& x) { id = x; }
    TicketStatus() {}
    QVariantMap toMap() const;
    static TicketStatus fromMap(const QVariantMap&);
    
};


struct TicketTimesheets {
    QVariant    id;
    QVariant    ticket;
    QVariant    user;
    QDateTime   date_from;
    QDateTime   date_to;

    TicketTimesheets(const QVariant& x) { id = x; }
    TicketTimesheets() {}
    QVariantMap toMap() const;
    static TicketTimesheets fromMap(const QVariantMap&);
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

};


struct TicketsVw : Tickets {
    QList<Dbt::TicketTimesheets> timesheets;
    QList<Dbt::TicketStatus>     statuses;
    QList<Dbt::TicketFiles>      files;
    QList<Dbt::TicketValues>     values;

    QVariantMap toMap() const Q_DECL_OVERRIDE;
    static Tickets fromMap(const QVariantMap&);
    TicketsVw() : Tickets() {}
    TicketsVw(const Tickets& x) {
        ticket = x.ticket;
        category = x.category;
        date = x.date;
        price = x.price;
        description = x.description ;
        user = x.user;
        }
};


}

#endif

