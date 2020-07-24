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
    //
    Users() { user = 0; }
    QVariantMap toMap() const;
};


struct Categories {
    QString     category;           // primary key
    QString     parent_category;
    QString     description;
    QVariantMap toMap() const;
};


struct StatusOrder {
    QString     category;
    QVariant    previous_status;
    QVariant    next_status;
    QVariantMap toMap() const;
};


struct Statuses {
    QString     status;
    QString     description;
    QString     abbreviation;
    QVariantMap toMap() const;
};


struct Tickets {
    QVariant    ticket;
    QVariant    category;
    QVariant    date;
    QVariant    price;
    QString     description;
    QVariant    rfid_read_id;
    QVariant    rfid_read_text;
    QVariant    rfid_written_id;
    QVariant    rfid_written_text;
    QVariant    qrcode_read;
    QVariant    qrcode_added;
    QVariant    gps_longitude;
    QVariant    gps_latitude;

    QVariantMap toMap() const;
};


}

#endif

