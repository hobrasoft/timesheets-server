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

    QVariantMap toMap() const;
    static Statuses fromMap(const QVariantMap&);
    Statuses(const QString& id) { status = id; }
    Statuses() {}
};


struct Tickets {
    QVariant    ticket;
    QVariant    category;
    QDateTime   date;
    QVariant    price;
    QString     description;
    int         user;

    QVariantMap toMap() const;
    Tickets() { user = 0; }
};


struct TicketFiles {
    QVariant    id;
    QVariant    ticket;
    QString     type;
    QByteArray  file;
    QVariantMap toMap() const;
};


struct TicketValues {
    QVariant    id;
    QVariant    ticket;
    QString     name;
    QString     value;
    QVariantMap toMap() const;
};


struct TicketStatus {
    QVariant    id;
    QVariant    ticket;
    QVariant    user;
    QString     user_name;
    QDateTime   date;   
    QString     description;
    QVariant    status;

    QVariantMap toMap() const;
};


struct TicketTimesheets {
    QVariant    id;
    QVariant    ticket;
    QDateTime   date_from;
    QDateTime   date_to;

    QVariantMap toMap() const;
};


struct TicketsVw : Tickets {
    QList<Dbt::TicketTimesheets> timesheets;
    QList<Dbt::TicketStatus>     ticket;
    QList<Dbt::TicketFiles>      files;
    QList<Dbt::TicketValues>     values;
};


}

#endif

