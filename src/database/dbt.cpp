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
    data["description"] = description;
    return data;
}


QVariantMap StatusOrder::toMap() const {
    QVariantMap data;
    data["category"] = category;
    data["previous_status"] = previous_status;
    data["next_status"] = next_status;
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

    data["ticket"] = ticket;
    data["category"] = category;
    data["date"] = date;
    data["price"] = price;
    data["description"] = description;
    data["rfid_read_id"] = rfid_read_id;
    data["rfid_read_text"] = rfid_read_text;
    data["rfid_written_id"] = rfid_written_id;
    data["rfid_written_text"] = rfid_written_text;
    data["qrcode_read"] = qrcode_read;
    data["qrcode_added"] = qrcode_added;
    data["gps_longitude"] = gps_longitude;
    data["gps_latitude"] = gps_latitude;

    return data;
}


QVariantMap TicketStatus::toMap() const {
    QVariantMap data;

    data["note"] = note;
    data["ticket"] = ticket;
    data["user"] = user;
    data["user_name"] = user_name;
    data["date"] =   date;   
    data["description"] = description;
    data["status"] = status;

    return data;
}

