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
    //
    Users() { user = 0; }
    QVariantMap toMap() const;
};


}

#endif

