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
    data["parent_category"] = parent_category;
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


QVariantMap TicketStatus::toMap() const {
    QVariantMap data;

    data["id"] = null(id);
    data["ticket"] = null(ticket);
    data["user"] = null(user);
    data["date"] = date;   
    data["description"] = description;
    data["status"] = null(status);

    return data;
}


TicketStatus TicketStatus::fromMap(const QVariantMap& data) {
    TicketStatus x;

    x.id = data["id"];
    x.ticket = data["ticket"];
    x.user = data["user"];
    x.date = data["date"].toDateTime();
    x.description = data["description"].toString();
    x.status = data["status"];

    return x;
}


