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
    return data;
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
    data["category"] = null(category);
    data["previous_status"] = null(previous_status);
    data["next_status"] = null(next_status);
    return data;
}


QVariantMap Statuses::toMap() const {
    QVariantMap data;
    data["status"] = status;
    data["description"] = description;
    data["abbreviation"] = abbreviation;
    data["color"] = color;
    return data;
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
    data["user_name"] = user_name;
    data["date"] = date;   
    data["description"] = description;
    data["status"] = null(status);

    return data;
}

