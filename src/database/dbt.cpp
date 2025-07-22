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


ServerInfo ServerInfo::fromMap(const QVariantMap& data) {
    ServerInfo x;
    x.name = data["name"].toString();
    x.description = data["description"].toString();
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
    data["description_tree"] = description_tree;
    data["price"] = price;
    data["users"] = users;
    return data;
}


Categories Categories::fromMap(const QVariantMap& data) {
    Categories x;
    x.category = data["category"].toString();
    x.parent_category = data["parent_category"].toString();
    x.description = data["description"].toString();
    x.price = data["price"].toDouble();
    x.users = data["users"].toList();
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


QVariantMap OverviewList::toMap() const {
    QVariantMap data;
    data["key"] = key;
    data["category"] = category.toMap();
    data["statuses"] = toList(statuses);
    return data;
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



QVariantMap CategoriesOverview::toMap() const {
    QVariantMap data;
    data["type"] = type;
    data["depth"] = depth;
    data["tickets_count"] = tickets_count;
    data["category"] = category;
    data["description"] = description;
    data["price"] = price;
    data["time"] = time;
    data["ordering"] = ordering;
    return data;
}


AppendStatuses AppendStatuses::fromMap(const QVariantMap& data) {
    AppendStatuses x;
    x.categories = data["categories"].toList();
    x.recent_status  = data["recent_status"].toList();
    x.status = data["status"].toString();
    x.description = data["description"].toString();
    return x;
}


QVariantMap AppendStatuses::toMap() const {
    QVariantMap data;
    data["categories"] = categories;
    data["recent_status"] = recent_status;
    data["status"] = status;
    data["description"] = description;
    return data;
}


QVariantMap Departments::toMap() const {
    QVariantMap data;
    data["department"] = department;
    data["abbr"] = abbr;
    data["description"] = description;
    return data;
}


Departments Departments::fromMap(const QVariantMap& data) {
    Departments x;
    x.department  = data["department"].toInt();
    x.abbr        = data["abbr"].toString();
    x.description = data["description"] .toString();
    return x;
}

QVariantMap EventTypes::toMap() const {
    QVariantMap data;
    data["event_type"] = event_type;
    data["description"] = description;
    data["event_type"] = event_type;
    data["description"] = description;
    data["end_state"] = end_state;
    data["passage"] = passage;
    data["arrival"] = arrival;
    data["vacation"] = vacation;
    data["sick_leave"] = sick_leave;
    data["compensatory_leave"] = compensatory_leave;
    data["business_trip"] = business_trip;
    data["break_time"] = break_time;
    data["unpaid_leave"] = unpaid_leave;
    data["sick_care"] = sick_care;
    data["doctor"] = doctor;
    data["paid_obstacle"] = paid_obstacle;
    return data;
}

EventTypes EventTypes::fromMap(const QVariantMap& data) {
    EventTypes x;
    x.event_type = data["event_type"].toString();
    x.description = data["description"].toString();
    x.end_state = data["end_state"].toBool();
    x.passage = data["passage"].toBool();
    x.arrival = data["arrival"].toBool();
    x.vacation = data["vacation"].toBool();
    x.sick_leave = data["sick_leave"].toBool();
    x.compensatory_leave = data["compensatory_leave"].toBool();
    x.business_trip = data["business_trip"].toBool();
    x.break_time = data["break_time"].toBool();
    x.unpaid_leave = data["unpaid_leave"].toBool();
    x.sick_care = data["sick_care"].toBool();
    x.doctor = data["doctor"].toBool();
    x.paid_obstacle = data["paid_obstacle"].toBool();
    return x;
}

QVariantMap Events::toMap() const {
    QVariantMap data;
    data["event"] = event;
    data["error"] = error;
    data["date"] = date;
    data["event_type"] = event_type;
    data["event_description"] = event_description;
    data["employee"] = employee;
    data["firstname"] = firstname;
    data["surname"] = surname;
    data["valid"] = valid;
    data["user_edited"] = user_edited;
    data["user_edited_name"] = user_edited_name;
    data["note"] = note;
    return data;
}

Events Events::fromMap(const QVariantMap& data) {
    Events x;
    x.event = data["event"].toInt(); 
    x.date = data["date"].toDateTime();
    x.event_type = data["event_type"].toString();
    x.employee = data["employee"].toInt(); 
    x.valid = data["valid"].toBool(); 
    x.user_edited = data["user_edited"].toInt();
    x.note = data["note"].toString();
    return x;
}

QVariantMap DepartmentHasManager::toMap() const {
    QVariantMap data;
    data["department"] = department;
    data["user"] = user;
    return data;
}

DepartmentHasManager DepartmentHasManager::fromMap(const QVariantMap& data) {
    DepartmentHasManager x;
    x.department = data["department"].toInt();
    x.user = data["user"].toInt();
    return x;
}

QVariantMap DepartmentHasMember::toMap() const {
    QVariantMap data;
    data["department"] = department;
    data["employee"] = employee;
    return data;
}

DepartmentHasMember DepartmentHasMember::fromMap(const QVariantMap& data) {
    DepartmentHasMember x;
    x.department = data["department"].toInt();
    x.employee = data["employee"].toInt();
    return x;
}

QVariantMap EmployeeCanOpenDoor::toMap() const {
    QVariantMap data;
    data["employee"] = employee;
    data["door"] = door;
    return data;
}

EmployeeCanOpenDoor EmployeeCanOpenDoor::fromMap(const QVariantMap& data) {
    EmployeeCanOpenDoor x;
    x.employee = data["employee"].toInt();
    x.door = data["door"].toInt();
    return x;
}

QVariantMap EmployeeHasRfid::toMap() const {
    QVariantMap data;
    data["employee"] = employee;
    data["rfid"] = rfid;
    return data;
}

EmployeeHasRfid EmployeeHasRfid::fromMap(const QVariantMap& data) {
    EmployeeHasRfid x;
    x.employee = data["employee"].toInt();
    x.rfid = data["rfid"].toInt();
    return x;
}

QVariantMap Employees::toMap() const {
    QVariantMap data;
    data["employee"] = employee;
    data["firstname"] = firstname;
    data["surname"] = surname;
    data["active"] = active;
    data["user"] = user;
    data["login"] = login;
    data["work_hours_mode"] = work_hours_mode;
    data["rounding_interval"] = rounding_interval;
    data["saturdays_paid"] = saturdays_paid;
    data["sundays_paid"] = sundays_paid;
    data["auto_breaks"] = auto_breaks;
    data["overtime_paid"] = overtime_paid;
    return data;
}

Employees Employees::fromMap(const QVariantMap& data) {
    Employees x;
    x.employee = data["employee"].toInt();
    x.firstname = data["firstname"].toString();
    x.surname = data["surname"].toString();
    x.active = data["active"].toBool();
    x.user = data["user"].toInt();
    x.login = data.value("login").toString();
    x.work_hours_mode = data["work_hours_mode"].toString();
    x.rounding_interval = data["rounding_interval"].toString();
    x.saturdays_paid = data["saturdays_paid"].toBool();
    x.sundays_paid = data["sundays_paid"].toBool();
    x.auto_breaks = data["auto_breaks"].toBool();
    x.overtime_paid = data["overtime_paid"].toBool();
    return x;
}


QVariantMap Doors::toMap() const {
    QVariantMap data;
    data["door"] = door;
    data["description"] = description;
    return data;
}


Doors Doors::fromMap(const QVariantMap& data) {
    Doors x;
    x.door = data["door"].toInt();
    x.description = data["description"].toString();
    return x;
}

QVariantMap Rfids::toMap() const {
    QVariantMap data;
    data["rfid"] = rfid;
    data["rfid_id"] = rfid_id;
    data["valid"] = valid;
    data["note"] = note;
    data["employee"] = employee;
    data["name"] = name;
    data["surname"] = surname;
    return data;
}

Rfids Rfids::fromMap(const QVariantMap& data) {
    Rfids x;
    x.rfid = data["rfid"].toInt();
    x.rfid_id = data["rfid_id"].toString();
    x.valid = data["valid"].toBool();
    x.note = data["note"].toString();
    x.employee = data["employee"].toInt();
    return x;
}

QVariantMap Holidays::toMap() const {
    QVariantMap data;
    data["date"] = date;
    data["description"] = description;
    return data;
}

Holidays Holidays::fromMap(const QVariantMap& data) {
    Holidays x;
    x.date = data["date"].toDate();
    x.description = data["description"].toString();
    return x;
}

QVariantMap WorkCalendar::toMap() const {
    QVariantMap data;
    data["period"] = period;
    data["working_days"] = working_days;
    data["holidays"] = holidays;
    data["hours8"] = hours8;
    data["hours85"] = hours85;
    return data;
}

WorkCalendar WorkCalendar::fromMap(const QVariantMap& data) {
    WorkCalendar x;
    x.period = data["period"].toDate();
    x.working_days = data["working_days"].toInt();
    x.holidays = data["holidays"].toInt();
    x.hours8 = data["hours8"].toString();
    x.hours85 = data["hours85"].toString();
    return x;
}

QVariantMap AttendanceSummary::toMap() const {
    QVariantMap data;
    data["employee"] = employee;
    data["month"] = month;
    data["days"] = days;
    data["arrival"] = arrival;
    data["vacation"] = vacation;
    data["sick_leave"] = sick_leave;
    data["compensatory_leave"] = compensatory_leave;
    data["business_trip"] = business_trip;
    data["break_time"] = break_time;
    data["unpaid_leave"] = unpaid_leave;
    data["sick_care"] = sick_care;
    data["paid_obstacle"] = paid_obstacle;
    data["doctor"] = doctor;
    data["afternoon"] = afternoon;
    data["night"] = night;
    data["sunday"] = sunday;
    data["saturday"] = saturday;
    data["holiday"] = holiday;
    data["calendar_working_days"] = calendar_working_days;
    data["calendar_holidays"] = calendar_holidays;
    data["firstname"] = firstname;
    data["surname"] = surname;
    data["days"] = days;
    data["locked"] = locked;
    data["locked_user"] = locked_user;
    data["locked_user_name"] = locked_user_name;

    return data;
}

AttendanceSummary AttendanceSummary::fromMap(const QVariantMap& data) {
    AttendanceSummary x;
    x.employee = data["employee"].toInt();
    x.month = data["month"].toDate();
    x.days = data["days"].toDouble();
    x.arrival = data["arrival"].toDouble();
    x.vacation = data["vacation"].toDouble();
    x.sick_leave = data["sick_leave"].toDouble();
    x.compensatory_leave = data["compensatory_leave"].toDouble();
    x.business_trip = data["business_trip"].toDouble();
    x.break_time = data["break_time"].toDouble();
    x.unpaid_leave = data["unpaid_leave"].toDouble();
    x.sick_care = data["sick_care"].toDouble();
    x.paid_obstacle = data["paid_obstacle"].toDouble();
    x.doctor = data["doctor"].toDouble();
    x.afternoon = data["afternoon"].toDouble();
    x.night = data["night"].toDouble();
    x.sunday = data["sunday"].toDouble();
    x.saturday = data["saturday"].toDouble();
    x.holiday = data["holiday"].toDouble();
    x.calendar_working_days = data["calendar_working_days"].toInt();
    x.calendar_holidays = data["calendar_holidays"].toInt();
    x.firstname = data["firstname"].toString();
    x.surname = data["surname"].toString();
    x.days = data["days"].toInt();
    x.locked = data["locked"].toBool();
    x.locked_user = data["locked_user"].toInt();
    x.locked_user_name = data["locked_user_name"].toString();

    return x;
}

QVariantMap AttendanceDays::toMap() const {
    QVariantMap data;
    data["dow"] = dow;
    data["holiday"] = holiday;
    data["holiday_description"] = holiday_description;
    data["start_event"] = null(start_event);
    data["start_date"] = start_date;
    data["start_event_type"] = start_event_type;
    data["start_event_description"] = start_event_description;
    data["start_event_note"] = start_event_note;
    data["start_event_error"] = start_event_error;
    data["start_user_edited"] = null(start_user_edited);
    data["start_user_edited_name"] = start_user_edited_name;
    data["end_event"] = null(end_event);
    data["end_date"] = end_date;
    data["end_event_type"] = end_event_type;
    data["end_event_description"] = end_event_description;
    data["end_event_note"] = end_event_note;
    data["end_event_error"] = end_event_error;
    data["end_user_edited"] = null(end_user_edited);
    data["end_user_edited_name"] = end_user_edited_name;
    data["end_generated"] = end_generated;
    data["rounded_hours"] = rounded_hours;
    data["cumulative_hours"] = cumulative_hours;
    data["should_be"] = should_be;
    data["should_be_cumulative"] = should_be_cumulative;
    return data;
}

AttendanceDays AttendanceDays::fromMap(const QVariantMap& data) {
    AttendanceDays x;
    x.dow = data["dow"].toInt();
    x.holiday = data["holiday"].toBool();
    x.holiday_description = data["holiday_description"].toString();
    x.start_event = data["start_event"];
    x.start_date = data["start_date"].toDateTime();
    x.start_event_type = data["start_event_type"].toString();
    x.start_event_description = data["start_event_description"].toString();
    x.start_event_note = data["start_event_note"].toString();
    x.start_event_error = data["start_event_error"].toString();
    x.start_user_edited = data["start_user_edited"];
    x.start_user_edited_name = data["start_user_edited_name"].toString();
    x.end_event = data["end_event"];
    x.end_date = data["end_date"].toDateTime();
    x.end_event_type = data["end_event_type"].toString();
    x.end_event_description = data["end_event_description"].toString();
    x.end_event_note = data["end_event_note"].toString();
    x.end_event_error = data["end_event_error"].toString();
    x.end_user_edited = data["end_user_edited"];
    x.end_user_edited_name = data["end_user_edited_name"].toString();
    x.end_generated = data["end_generated"].toBool();
    x.rounded_hours = data["rounded_hours"].toDouble();
    x.cumulative_hours = data["cumulative_hours"].toDouble();
    x.should_be = data["should_be"].toDouble();
    x.should_be_cumulative = data["should_be_cumulative"].toDouble();
    return x;
}

QList<AttendanceDays> AttendanceDays::fromList(const QVariantList& list) {
    QList<AttendanceDays> x;
    QListIterator<QVariant> iterator(list);
    while (iterator.hasNext()) {
        x << AttendanceDays::fromMap(iterator.next().toMap());
    }
    return x;
}

QVariantMap AttendanceChecklist::toMap() const {
    QVariantMap data;
    data["month"] = month;
    data["employee"] = employee.toMap();
    data["days"] = toList(days);
    data["summary_calculated"] = summary_calculated.toMap();
    data["summary_saved"] = summary_saved.toMap();
    return data;
}

AttendanceChecklist AttendanceChecklist::fromMap(const QVariantMap& data) {
    AttendanceChecklist x;
    x.month = data["month"].toDate();
    x.employee = Employees::fromMap(data["employee"].toMap());
    x.days = AttendanceDays::fromList(data["days"].toList());
    x.summary_calculated = AttendanceSummary::fromMap(data["summary_calculated"].toMap());
    x.summary_saved = AttendanceSummary::fromMap(data["summary_saved"].toMap());
    return x;
}


