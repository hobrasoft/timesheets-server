/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "dbt.h"

using namespace Dbt;

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
    data["abbreviation"] = abbreviation;
    data["description"] = description;
    return data;
}
