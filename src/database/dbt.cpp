/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "dbt.h"
#include "pdebug.h"

using namespace Dbt;


/**
 * @brief Konvertuje null variant na invalid variant - QJsonDocument jinak konvertuje int null hodnoty špatně na nulu
 */
namespace Dbt { 
    static QVariant null(const QVariant& x) {
        if (x.isValid() && !x.isNull()) { return x; }
        return QVariant();
    }
}


QVariantMap Users::toMap() const {
    QVariantMap data;
    data["user"] = user;
    data["login"] = login;
    data["name"] = name;
    data["lang"] = lang;
    data["enabled"] = enabled;
    data["admin"] = admin;
    return data;
}


Users Users::fromMap(const QVariantMap& data) {
    Users x;
    x.user = data["user"].toInt();
    x.login = data["login"].toString();
    x.name = data["name"].toString();
    x.lang = data["lang"].toString();
    x.enabled = data["enabled"].toBool();
    x.admin = data["admin"].toBool();
    return x;
}


QVariantMap Categories::toMap() const {
    QVariantMap data;
    data["category"] = category;
    data["parent_category"] = null(parent_category);
    data["description"] = description;
    data["price"] = price;
    return data;
}


Categories Categories::fromMap(const QVariantMap& data) {
    Categories x;
    x.category = data["category"].toString();
    x.parent_category = data["parent_category"].toString();
    x.description = data["description"].toString();
    x.price = data["price"].toDouble();
    return x;
}


QVariantMap StatusOrder::toMap() const {
    QVariantMap data;
    data["id"] = null(id);
    data["category"] = null(category);
    data["previous_status"] = null(previous_status);
    data["next_status"] = null(next_status);
    return data;
}


StatusOrder StatusOrder::fromMap(const QVariantMap& data) {
    StatusOrder x;
    x.id = data["id"];
    x.category = data["category"];
    x.previous_status = data["previous_status"];
    x.next_status = data["next_status"];
    return x;
}


QVariantMap Statuses::toMap() const {
    QVariantMap data;
    data["status"] = status;
    data["description"] = description;
    data["abbreviation"] = abbreviation;
    data["color"] = color;
    data["closed"] = closed;
    return data;
}


Statuses Statuses::fromMap(const QVariantMap& data) {
    Statuses x;
    x.status = data["status"].toString();
    x.description = data["description"].toString();
    x.abbreviation = data["abbreviation"].toString();
    x.color = data["color"].toString();
    x.closed= data["closed"].toBool();
    return x;
}


Tickets Tickets::fromMap(const QVariantMap& map) {
    Tickets x;
    x.ticket = map["ticket"];
    x.category = map["category"];
    x.price = map["price"];
    x.date = map["date"].toDateTime();
    x.user = map["user"].toInt();
    x.description = map["description"].toString();
    return x;
}


QVariantMap Tickets::toMap() const {
    QVariantMap data;

    data["ticket"] = null(ticket);
    data["category"] = null(category);
    data["date"] = date;
    data["price"] = null(price);
    data["description"] = description;
    data["user"] = user;

    return data;
}


QVariantMap TicketsVw::toMap() const {
    QVariantMap data = Tickets::toMap();
    data["timesheets"] = toList(timesheets);
    data["statuses"] = toList(statuses);
    data["values"] = toList(values);
    data["files"] = toList(files);
    return data;
}


TicketsVw TicketsVw::fromMap(const QVariantMap& map) {
    TicketsVw x = Tickets::fromMap(map);
    x.timesheets = TicketTimesheets::fromList(map["timesheets"].toList());
    x.statuses = TicketStatus::fromList(map["statuses"].toList());
    x.values = TicketValues::fromList(map["values"].toList());
    x.files = TicketFiles::fromList(map["files"].toList());
    return x;
}


Mutable Mutable::fromMap(const QVariantMap& data) {
    Mutable x;
    x.id = data["id"].toInt();
    x.user = data["user"];
    x.ticket = data["ticket"];
    return x;
}


QVariantMap Mutable::toMap() const {
    QVariantMap data;
    data["id"] = id;
    data["user"] = user;
    data["ticket"] = ticket;
    // data["modified"] = modified;
    // data["created"] = created;
    return data;
}


TicketTimesheets TicketTimesheets::fromMap(const QVariantMap& data) {
    TicketTimesheets x = Mutable::fromMap(data);;
    x.date_from = data["date_from"].toDateTime();
    x.date_to = data["date_to"].toDateTime();
    return x;
}


TicketFiles TicketFiles::fromMap(const QVariantMap& data) {
    TicketFiles x = Mutable::fromMap(data);
    x.date = data["date"].toDateTime();
    x.name = data["name"].toString();
    x.type = data["type"].toString();
    x.content = data["content"].toByteArray();
    return x;
}


TicketValues TicketValues::fromMap(const QVariantMap& data) {
    TicketValues x = Mutable::fromMap(data);
    x.date = data["date"].toDateTime();
    x.name = data["name"].toString();
    x.value = data["value"].toString();
    return x;
}


TicketStatus TicketStatus::fromMap(const QVariantMap& data) {
    TicketStatus x = Mutable::fromMap(data);
    x.date = data["date"].toDateTime();;
    x.status = data["status"].toString();
    x.description = data["description"].toString();
    return x;
}


QVariantMap TicketTimesheets::toMap() const {
    QVariantMap data = Mutable::toMap();;
    data["date_from"] = date_from;
    data["date_to"] = date_to;
    return data;
}


QVariantMap TicketFiles::toMap() const {
    QVariantMap data = Mutable::toMap();;
    data["date"] = date;
    data["name"] = name;
    data["type"] = type;
    data["content"] = content;
    return data;
}


QVariantMap TicketValues::toMap() const {
    QVariantMap data = Mutable::toMap();;
    data["date"] = date;
    data["name"] = name;
    data["value"] = value;
    return data;
}


QVariantMap TicketStatus::toMap() const {
    QVariantMap data = Mutable::toMap();;
    data["status"] = status;
    data["date"] = date;
    data["description"] = description;
    return data;
}


QList<TicketTimesheets> TicketTimesheets::fromList(const QVariantList& list) {
    QList<TicketTimesheets> x;
    QListIterator<QVariant> iterator(list);
    while (iterator.hasNext()) {
        x << TicketTimesheets::fromMap(iterator.next().toMap());
        }
    return x;
}


QList<TicketValues> TicketValues::fromList(const QVariantList& list) {
    QList<TicketValues> x;
    QListIterator<QVariant> iterator(list);
    while (iterator.hasNext()) {
        x << TicketValues::fromMap(iterator.next().toMap());
        }
    return x;
}


QList<TicketFiles> TicketFiles::fromList(const QVariantList& list) {
    QList<TicketFiles> x;
    QListIterator<QVariant> iterator(list);
    while (iterator.hasNext()) {
        x << TicketFiles::fromMap(iterator.next().toMap());
        }
    return x;
}


QList<TicketStatus> TicketStatus::fromList(const QVariantList& list) {
    QList<TicketStatus> x;
    QListIterator<QVariant> iterator(list);
    while (iterator.hasNext()) {
        x << TicketStatus::fromMap(iterator.next().toMap());
        }
    return x;
}


