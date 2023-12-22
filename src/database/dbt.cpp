/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "dbt.h"
#include "msettings.h"
#include "json.h"
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


QVariantMap ClientSettings::toMap() const {
    QVariantMap x;
    x["multiple_timesheets"] = multiple_timesheets;
    x["show_price"] = show_price;
    x["can_change_category"] = can_change_category;
    x["edit_categories"] = edit_categories;
    x["show_multiple_timesheets"] = show_multiple_timesheets;
    x["show_show_price"] = show_show_price;
    x["show_can_change_category"] = show_can_change_category;
    x["show_edit_categories"] = show_edit_categories;
    return x;
}


ClientSettings ClientSettings::fromMap(const QVariantMap& map) {
    ClientSettings x;
    x.multiple_timesheets = map["multiple_timesheets"].toBool();
    x.show_price = map["show_price"].toBool();
    x.can_change_category = map["can_change_category"].toBool();
    x.edit_categories = map["edit_categories"].toBool();
    x.show_multiple_timesheets = map["show_multiple_timesheets"].toBool();
    x.show_show_price = map["show_show_price"].toBool();
    x.show_can_change_category = map["show_can_change_category"].toBool();
    x.show_edit_categories = map["show_edit_categories"].toBool();
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


UsersCategories UsersCategories::fromMap(const QVariantMap& data) {
    UsersCategories x;
    x.id = data["id"].toInt();
    x.user = data["user"].toInt();
    x.category = data["category"].toString();
    return x;
}


QVariantMap UsersCategories::toMap() const {
    QVariantMap data;
    data["id"] = id;
    data["user"] = user;
    data["category"] = null(category);
    return data;
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
    data["can_be_run"] = can_be_run;
    data["ignored"] = ignored;
    if (can_have_next) {
        data["next"] = toList(next);
        }
    return data;
}


Statuses Statuses::fromMap(const QVariantMap& data) {
    Statuses x;
    x.status = data["status"].toString();
    x.description = data["description"].toString();
    x.abbreviation = data["abbreviation"].toString();
    x.color = data["color"].toString();
    x.closed= data["closed"].toBool();
    x.can_be_run = data["can_be_run"].toBool();
    x.ignored = data["ignored"].toBool();
    if (data.contains("next")) {
        const QVariantList next = data["next"].toList();
        for (int i=0; i<next.size(); i++) {
            x.next << Statuses::fromMap(next[i].toMap());
            }
        }
    return x;
}


QVariantMap StatusTemplates::toMap() const {
    QVariantMap data;
    data["id"] = id;
    data["status"] = status;
    data["category"] = category;
    data["code"] = code;
    data["title"] = title;
    data["description"] = description;
    return data;
}


Tickets Tickets::fromMap(const QVariantMap& map) {
    Tickets x;
    x.ticket = map["ticket"];
    x.category = map["category"];
    x.price = map["price"].toDouble();
    x.date = map["date"].toDateTime();
    x.user = map["user"].toInt();
    x.description = map["description"].toString();
    x.created = map["created"].toBool();
    x.modified = map["modified"].toBool();
    return x;
}


QVariantMap Tickets::toMap() const {
    QVariantMap data;

    data["ticket"] = null(ticket);
    data["category"] = null(category);
    data["date"] = date;
    data["price"] = price;
    data["description"] = description;
    data["user"] = user;
    data["created"] = created;
    data["modified"] = modified;

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
    TicketTimesheets x = Mutable::fromMap(data);
    x.date_from = data["date_from"].toDateTime();
    x.date_to = data["date_to"].toDateTime();
    return x;
}


TicketFiles TicketFiles::fromMap(const QVariantMap& data) {
    TicketFiles x = Mutable::fromMap(data);
    x.date = data["date"].toDateTime();
    x.name = data["name"].toString();
    x.type = data["type"].toString();
    x.content = QByteArray::fromBase64(data["content"].toByteArray());
    return x;
}


TicketValues TicketValues::fromMap(const QVariantMap& data) {
    TicketValues x = Mutable::fromMap(data);
    x.date = data["date"].toDateTime();
    x.name = data["name"].toString();
    x.value = data["value"];
    PDEBUG << data;
    PDEBUG << x.value;
    return x;
}


TicketStatus TicketStatus::fromMap(const QVariantMap& data) {
    TicketStatus x = Mutable::fromMap(data);
    x.date = data["date"].toDateTime();
    x.status = data["status"].toString();
    x.description = data["description"].toString();
    x.description2 = data["description2"].toMap();
    return x;
}


QVariantMap TicketTimesheets::toMap() const {
    QVariantMap data = Mutable::toMap();
    data["date_from"] = date_from;
    data["date_to"] = date_to;
    return data;
}


QVariantMap TicketFiles::toMap() const {
    QVariantMap data = Mutable::toMap();
    data["date"] = date;
    data["name"] = name;
    data["type"] = type;
    data["content"] = content.toBase64();
    return data;
}


QVariantMap TicketValues::toMap() const {
    QVariantMap data = Mutable::toMap();
    data["date"] = date;
    data["name"] = name;
    data["value"] = value;
    return data;
}


QVariantMap TicketStatus::toMap() const {
    QVariantMap data = Mutable::toMap();
    data["status"] = status;
    data["date"] = date;
    data["description"] = description;
    data["description2"] = description2;
    data["status_description"] = status_description;
    data["status_color"] = status_color;
    data["status_can_be_run"] = status_can_be_run;
    data["status_ignored"] = status_ignored;
    data["status_closed"] = status_closed;
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


QVariantMap Overview::Days::toMap() const {
    QVariantMap data;
    data["ticket"] = ticket;
    data["description"] = description;
    data["user"] = user;
    data["user_name"] = user_name;
    data["date"] = date;
    data["hour_price"] = hour_price;
    data["duration"] = duration;
    data["price"] = price;
    return data;
}


QVariantMap Overview::Sum::toMap() const {
    QVariantMap data;
    data["duration"] = duration;
    data["price"] = price;
    return data;
}


QVariantMap Overview::Tickets::toMap() const {
    QVariantMap data;
    data["ticket"] = ticket;
    data["description"] = description;
    data["user"] = user;
    data["user_name"] = user_name;
    data["hour_price"] = hour_price;
    data["duration"] = duration;
    data["price"] = price;
    data["status"] = status;
    return data;
}


QVariantMap Overview::TicketsSum::toMap() const {
    QVariantMap data;
    data["ticket"] = ticket;
    data["description"] = description;
    data["duration"] = duration;
    data["price"] = price;
    data["status"] = status;
    return data;
}


QVariantMap Overview::StatusSum::toMap() const {
    QVariantMap data;
    data["duration"] = duration;
    data["price"] = price;
    data["status"] = status;
    return data;
}


QVariantMap Overview::UserSum::toMap() const {
    QVariantMap data;
    data["duration"] = duration;
    data["price"] = price;
    data["user_name"] = user_name;
    return data;
}


QVariantMap Overview::toMap() const {
    QVariantMap data;
    data["category"] = category.toMap();
    data["tickets"] = toList(tickets);

    QVariantMap xdays;
    xdays["records"] = toList(days);
    xdays["sum"] = sum.toMap();
    data["days"] = xdays;

    QVariantMap xtickets; 
    xtickets["records"] = toList(tickets);
    xtickets["sum"] = toList(ticketsSum);
    data["tickets"] = xtickets;

    data["status_sum"] = toList(statusSum);
    data["people_sum"] = toList(userSum);

    data["id"] = id;

    return data;    
};


