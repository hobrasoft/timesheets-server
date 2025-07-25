/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _DBT_H_
#define _DBT_H_

#include <QString>
#include <QDateTime>
#include <QDate>
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


struct ClientSettings {
    bool    multiple_timesheets;
    bool    show_price;
    bool    can_change_category;
    bool    edit_categories;
    bool    show_multiple_timesheets;
    bool    show_show_price;
    bool    show_can_change_category;
    bool    show_edit_categories;
    ClientSettings() {
        multiple_timesheets = true;
        show_price = true;
        can_change_category = true;
        edit_categories = true;
        show_multiple_timesheets = true;
        show_show_price = true;
        show_can_change_category = true;
        show_edit_categories = true;
        }
    QVariantMap toMap() const;
    static ClientSettings fromMap(const QVariantMap&);
};


struct ServerInfo {
    QString     name;
    QString     description;

    static ServerInfo fromMap(const QVariantMap&);
};


struct Categories {
    QString         category;           // primary key
    QString         parent_category;
    QString         description;
    QString         description_tree;
    double          price;
    QVariantList    users;

    QVariantMap toMap() const;
    static Categories fromMap(const QVariantMap&);
    Categories() { price = 0; }
    Categories(const QString& id) { price = 0; category = id; }
};


struct UsersCategories {
    int         id;
    QString     category;
    int         user;

    QVariantMap toMap() const;
    static UsersCategories fromMap(const QVariantMap&);
    UsersCategories() { id = -1; user = -1; }
    UsersCategories(int i) { id = i; }
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
    bool        closed;
    bool        can_be_run;
    bool        ignored;
    bool        can_have_next;   // Není součástí tabulky, flag pro generování next
    QList<Statuses> next;

    QVariantMap toMap() const;
    static Statuses fromMap(const QVariantMap&);
    Statuses(const QString& id) { clear(); status = id; }
    Statuses() { clear(); }
    void clear() { 
        closed = false; 
        can_be_run = false; 
        ignored = false; 
        can_have_next = false; 
        status = QString(); 
        description = QString(); 
        abbreviation = QString(); 
        color = QString(); 
        next.clear();
        }
};


struct StatusTemplates {
    int         id;
    QString     status;
    QString     category;
    QString     code;
    QString     title;
    QString     description;

    QVariantMap toMap() const;
    StatusTemplates() { id = -1; }
};


// Tabulky modifikovatelné
struct Mutable {
    int         id;
    QVariant    user;
    QVariant    ticket;
    bool        modified;
    bool        created;
    //
    virtual ~Mutable() {}
    virtual QVariantMap toMap() const;
    static Mutable fromMap(const QVariantMap&);
    Mutable() { id = 0; modified = false; created = false; }
    Mutable(const QVariant& x) { id = x.toInt(); modified = false; created = false; }
    Mutable(const Mutable& x) { operator=(x); }
    Mutable& operator=(const Mutable& x) {
        id = x.id;
        user = x.user;
        ticket = x.ticket;
        modified = x.modified;
        created = x.created;
        return *this;
        }
};


struct TicketStatus : Mutable {
    QDateTime   date;
    QString     status;
    QString     description;
    QVariantMap description2;
    QString     status_description;
    QString     status_color;
    bool        status_closed;
    bool        status_can_be_run;
    bool        status_ignored;
    //
    virtual ~TicketStatus() {}
    QVariantMap toMap() const Q_DECL_OVERRIDE;
    static TicketStatus fromMap(const QVariantMap&);
    static QList<TicketStatus> fromList(const QVariantList&);
    TicketStatus() : Mutable() { status_closed = false; status_can_be_run = true; status_ignored = false; }
    TicketStatus(const QVariant& x) : Mutable(x) { status_closed = false; status_can_be_run = true; status_ignored = false; }
    TicketStatus(const Mutable& x) : Mutable(x) { status_closed = false; status_can_be_run = true; status_ignored = false; }
    TicketStatus(const TicketStatus& x) : Mutable() { operator=(x); }
    TicketStatus& operator=(const TicketStatus& x) {
        Mutable::operator=(x);
        date = x.date;
        status = x.status;
        description = x.description;
        description2 = x.description2;
        status_description = x.status_description;
        status_color = x.status_color;
        status_closed = x.status_closed;
        status_can_be_run = x.status_can_be_run;
        status_ignored = x.status_ignored;
        return *this;
        }
};


struct TicketTimesheets : Mutable {
    QDateTime   date_from;
    QDateTime   date_to;
    //
    virtual ~TicketTimesheets() {}
    QVariantMap toMap() const Q_DECL_OVERRIDE;
    static TicketTimesheets fromMap(const QVariantMap&);
    static QList<TicketTimesheets> fromList(const QVariantList&);
    TicketTimesheets() : Mutable() {}
    TicketTimesheets(const QVariant& x) : Mutable(x) {}
    TicketTimesheets(const Mutable& x) : Mutable(x) {}
    TicketTimesheets(const TicketTimesheets& x) : Mutable() { operator=(x); }
    TicketTimesheets& operator=(const TicketTimesheets& x) {
        Mutable::operator=(x);
        date_from = x.date_from;
        date_to = x.date_to;
        return *this;
        }

};


struct TicketFiles : Mutable {
    QDateTime   date;
    QString     name;
    QString     type;
    QByteArray  content;

    virtual ~TicketFiles() {}
    QVariantMap toMap() const Q_DECL_OVERRIDE;
    static TicketFiles fromMap(const QVariantMap&);
    static QList<TicketFiles> fromList(const QVariantList&);
    TicketFiles() : Mutable() {}
    TicketFiles(const QVariant& x) : Mutable(x) {}
    TicketFiles(const Mutable& x) : Mutable(x) {}
    TicketFiles(const TicketFiles& x) : Mutable() { operator=(x); }
    TicketFiles& operator=(const TicketFiles& x) {
        Mutable::operator=(x);
        date = x.date;
        name = x.name;
        type = x.type;
        content = x.content;
        return *this;
        }
};


struct TicketValues : Mutable {
    QDateTime   date;
    QString     name;
    QVariant    value;

    virtual ~TicketValues() {}
    QVariantMap toMap() const Q_DECL_OVERRIDE;
    static TicketValues fromMap(const QVariantMap&);
    static QList<TicketValues> fromList(const QVariantList&);
    TicketValues() : Mutable() {}
    TicketValues(const QVariant& x) : Mutable(x) {}
    TicketValues(const Mutable& x) : Mutable(x) {}
    TicketValues(const TicketValues& x) : Mutable() { operator=(x); }
    TicketValues& operator=(const TicketValues& x) {
        Mutable::operator=(x);
        date = x.date;
        name = x.name;
        value = x.value;
        return *this;
        }

};


struct Tickets {
    QVariant    ticket;
    QVariant    category;
    QDateTime   date;
    double      price;
    QString     description;
    int         user;
    bool        created;
    bool        modified;

    virtual ~Tickets() {}
    virtual QVariantMap toMap() const;
    static Tickets fromMap(const QVariantMap&);
    Tickets() { user = 0; price = 0; created = false; modified = false; }
    Tickets(const QVariant& x) { ticket = x; price = 0; created = false;  modified = false; }
    Tickets(const Tickets& x) { operator=(x); }
    Tickets& operator=(const Tickets& x) {
        ticket = x.ticket;
        category = x.category;
        date = x.date;
        price = x.price;
        description = x.description ;
        user = x.user;
        created = x.created;
        modified = x.modified;
        return *this;
        }

};


struct TicketsVw : Tickets {
    QList<Dbt::TicketTimesheets> timesheets;
    QList<Dbt::TicketStatus>     statuses;
    QList<Dbt::TicketFiles>      files;
    QList<Dbt::TicketValues>     values;

    virtual ~TicketsVw() {}
    QVariantMap toMap() const Q_DECL_OVERRIDE;
    static TicketsVw fromMap(const QVariantMap&);
    TicketsVw() : Tickets() {}
    TicketsVw(const QVariant& x) : Tickets(x) { }
    TicketsVw(const Tickets& x) : Tickets(x) {}
    TicketsVw(const TicketsVw& x) : Tickets() { operator=(x); }
    TicketsVw& operator=(const TicketsVw& x) {
        Tickets::operator=(x);
        timesheets = x.timesheets;
        statuses = x.statuses;
        files = x.files;
        values = x.values;
        return *this;
        }

};


struct CategoriesOverview {
    QString             type;
    int                 depth;
    QString             category;
    QString             description;
    int                 tickets_count;
    double              price;
    double              time;
    QString             ordering;

    CategoriesOverview() { depth = 0; price = 0; time = 0; }
    QVariantMap     toMap() const;
};


struct AppendStatuses {
    QVariantList        categories;         ///< categories which are appended with new statuse
    QVariantList        recent_status;      ///< recent statuses 
    QString             status;             ///< new status
    QString             description;        ///< new status description
    static AppendStatuses fromMap(const QVariantMap&);
    QVariantMap     toMap() const;
};


struct OverviewList {
    QString              key;
    Dbt::Categories      category;
    QList<Dbt::Statuses> statuses;

    OverviewList() {}
    OverviewList(const QString& x) { key = x; } 
    QVariantMap     toMap() const;
};


struct Overview {
    struct Days {
        int         ticket;
        QString     description;
        int         user;
        QString     user_name;
        QDateTime   date;
        double      hour_price;
        double      duration;
        double      price;
        QVariantMap toMap() const;
        Days() { ticket = 0; user = 0; hour_price = 0; duration = 0; price = 0; }
        };
    struct Sum {
        double      duration;
        double      price;
        QVariantMap toMap() const;
        Sum() { duration = 0; price = 0; }
        };
    struct Tickets {
        int         ticket;
        QString     description;
        int         user;
        QString     user_name;
        double      hour_price;
        double      duration;
        double      price;
        QString     status;
        QVariantMap toMap() const;
        Tickets() { ticket = 0; user = 0; hour_price = 0; duration = 0; price = 0; }
        };
    struct TicketsSum {
        int         ticket;
        QString     description;
        double      duration;
        double      price;
        QString     status;
        QVariantMap toMap() const;
        TicketsSum() { ticket = 0; duration = 0; price = 0; }
        };
    struct StatusSum {
        double      duration;
        double      price;
        QString     status;
        QVariantMap toMap() const;
        StatusSum() { duration = 0; price = 0; }
        };
    struct UserSum {
        double      duration;
        double      price;
        QString     user_name;
        QVariantMap toMap() const;
        UserSum() { duration = 0; price = 0; }
        };

    Categories          category;
    QList<Tickets>      tickets;        // group by ticket, user
    QList<TicketsSum>   ticketsSum;     // group by ticket
    QList<StatusSum>    statusSum;      // group by status
    QList<UserSum>      userSum;        // group by users
    QList<Days>         days;           // group by date, user
    Sum                 sum;            // group by category
    QVariantMap         toMap() const;
    QString             id;
};

//---------------------------------------------------------------------------------------
// Docházka

struct Departments {
    int         department;
    QString     abbr;
    QString     description;

    Departments(int d) { department = d; }
    Departments() { department = 0; }
    static Departments fromMap(const QVariantMap&);
    QVariantMap toMap() const;
};

struct Doors {
    int         door;
    QString     description;

    Doors() { door = 0; }
    Doors(int x) { door = x; }
    static Doors fromMap(const QVariantMap&);
    QVariantMap toMap() const;
};

struct Rfids {
    int         rfid;
    QString     rfid_id;
    bool        valid;
    QString     note;
    int         employee;
    QString     name;
    QString     surname;

    static Rfids fromMap(const QVariantMap&);
    Rfids() { rfid = 0; valid = false; employee = 0; }
    Rfids(int id) { rfid = id; valid = false; employee = 0; }
    QVariantMap toMap() const;
};

/**
 * @brief Zaměstnanec používaný v REST API
 *
 * Struktura odpovídá JSON objektu vracenému z /api/v1/employees.
 *
 * Položky:
 * - employee: ID zaměstnance
 * - firstname: jméno
 * - surname: příjmení
 * - active: zda je zaměstnanec aktivní
 * - user: uživatelský účet (ID)
 * - login: přihlašovací jméno uživatele
 * - work_hours_mode: režim pracovní doby
 * - rounding_interval: zaokrouhlení docházky
 * - saturdays_paid: placené soboty
 * - sundays_paid: placené neděle
 * - auto_breaks: automatické přestávky
 * - overtime_paid: proplácené přesčasy
 */
struct Employees {
    int         employee;
    QString     firstname;
    QString     surname;
    bool        active;
    int         user;
    QString     login;
    QString     work_hours_mode;
    QString     rounding_interval;
    bool        saturdays_paid;
    bool        sundays_paid;
    bool        auto_breaks;
    bool        overtime_paid;

    Employees() {
        employee = 0;
        active = false;
        user = 0;
        login = "";
        saturdays_paid = false;
        sundays_paid = false;
        auto_breaks = false;
        overtime_paid = false;
    }
    Employees(int x) {
        employee = x;
        active = false;
        user = 0;
        login = "";
        saturdays_paid = false;
        sundays_paid = false;
        auto_breaks = false;
        overtime_paid = false;
    }
    static Employees fromMap(const QVariantMap&);
    QVariantMap toMap() const;
};

struct EventTypes {
    QString     event_type;
    QString     description;
    bool        end_state;              // odchod, ukončení předchozího stavu
    bool        passage;                // pruchod
    bool        arrival;                // příchod do práce
    bool        vacation;               // Dovolená
    bool        sick_leave;             // nemoc
    bool        compensatory_leave;     // náhradní volno
    bool        business_trip;          // Služební cesta
    bool        break_time;             // Přestávka
    bool        unpaid_leave;           // Neplacené volno
    bool        sick_care;              // Ošetřování člena rodiny
    bool        doctor;                 // Návštěva lékaře
    bool        paid_obstacle;          // Překázka v práci na straně zaměstnavatele


    EventTypes(const QString& id) {
        clear();
        event_type = id;
        }

    EventTypes() {
        clear();
        }

    static EventTypes fromMap(const QVariantMap&);
    QVariantMap toMap() const;

    void clear() {
        event_type.clear();
        end_state = false;
        passage = false;
        arrival = false;
        vacation = false;
        sick_leave = false;
        compensatory_leave = false;
        business_trip = false;
        break_time = false;
        unpaid_leave = false;
        sick_care = false;
        }
};

struct Events {
    int         event;
    QString     error;
    QDateTime   date;
    QString     event_type;
    QString     event_description;
    int         employee;
    QString     firstname;
    QString     surname;
    bool        valid;
    QVariant    user_edited;
    QString     user_edited_name;
    QString     note;

    Events(int id) { event = id; employee = 0; valid = false; }
    Events() { event = 0; employee = 0; valid = false; }
    static Events fromMap(const QVariantMap&);
    QVariantMap toMap() const;
};

struct DepartmentHasManager {
    int         department;
    int         user;

    DepartmentHasManager() { department = 0; user = 0; }
    DepartmentHasManager(int d, int u) { department = d; user = u; }
    static DepartmentHasManager fromMap(const QVariantMap&);
    QVariantMap toMap() const;
};

struct DepartmentHasMember {
    int         department;
    int         employee;

    DepartmentHasMember() { department = 0; employee = 0; }
    DepartmentHasMember(int d, int e) { department = d; employee = e; }
    static DepartmentHasMember fromMap(const QVariantMap&);
    QVariantMap toMap() const;
};

struct EmployeeCanOpenDoor {
    int         employee;
    int         door;

    EmployeeCanOpenDoor() { employee = 0; door = 0; }
    EmployeeCanOpenDoor(int e, int d) { employee = e; door = d; }
    static EmployeeCanOpenDoor fromMap(const QVariantMap&);
    QVariantMap toMap() const;
};

struct EmployeeHasRfid {
    int         employee;
    int         rfid;

    EmployeeHasRfid() { employee = 0; rfid = 0; }
    EmployeeHasRfid(int e, int r) { employee = e; rfid = r; }
    static EmployeeHasRfid fromMap(const QVariantMap&);
    QVariantMap toMap() const;
};

struct Holidays {
    QDate       date;
    QString     description;

    Holidays() { }
    Holidays(const QDate& d) { date = d; }
    static Holidays fromMap(const QVariantMap&);
    QVariantMap toMap() const;
};

struct WorkCalendar {
    QDate       period;
    int         working_days;
    int         holidays;
    QString     hours8;
    QString     hours85;

    WorkCalendar() { working_days = 0; holidays = 0; }
    WorkCalendar(const QDate& d) { period = d; working_days = 0; holidays = 0; }
    static WorkCalendar fromMap(const QVariantMap&);
    QVariantMap toMap() const;
};


struct AttendanceSummary {
    // double = interval (hours)
    QDate       month;
    int         employee;
    QString     firstname;          // view
    QString     surname;            // view
    int         days;
    bool        locked;
    int         locked_user;
    QString     locked_user_name;   // view
    double      arrival;            // work
    double      vacation;
    double      sick_leave;
    double      compensatory_leave;
    double      business_trip;
    double      break_time;
    double      unpaid_leave;
    double      sick_care;
    double      paid_obstacle;
    double      doctor;
    double      afternoon;
    double      night;
    double      sunday;
    double      saturday;
    double      holiday;
    int         calendar_working_days;
    int         calendar_holidays;
    AttendanceSummary() {
        month = QDate();
        employee = 0;
        locked = false;
        locked_user = 0;
        days = 0;
        arrival = 0;
        vacation = 0;
        sick_leave = 0;
        compensatory_leave = 0;
        business_trip = 0;
        break_time = 0;
        unpaid_leave = 0;
        sick_care = 0;
        paid_obstacle = 0;
        doctor = 0;
        afternoon = 0;
        night = 0;
        sunday = 0;
        saturday = 0;
        holiday = 0;
        calendar_working_days = 0;
        calendar_holidays = 0;
        }
    static AttendanceSummary fromMap(const QVariantMap&);
    QVariantMap toMap() const;
};


struct AttendanceDays {
    int         dow;                    // den v týdnu, odpovídá PgSql
    bool        holiday;                // Je svátek?
    QString     holiday_description;    // 
    // Začátek
    QVariant    start_event;
    QDateTime   start_date;
    QString     start_event_type;
    QString     start_event_description;
    QString     start_event_note;
    QString     start_event_error;
    QVariant    start_user_edited;
    QString     start_user_edited_name;
    // Konec
    QVariant    end_event;
    QDateTime   end_date;
    QString     end_event_type;
    QString     end_event_description;
    QString     end_event_note;
    QString     end_event_error;
    QVariant    end_user_edited;
    QString     end_user_edited_name;
    bool        end_generated;
    double      rounded_hours;          // interval (hours)
    double      cumulative_hours;       // interval (hours)
    double      should_be;              // interval (hours)
    double      should_be_cumulative;   // interval (hours)
    // 
    AttendanceDays() {
        end_generated = false;
        rounded_hours = 0;
        cumulative_hours = 0;
        should_be = 0;
        should_be_cumulative = 0;
        }
    static AttendanceDays fromMap(const QVariantMap&);
    static QList<AttendanceDays> fromList(const QVariantList&);
    QVariantMap toMap() const;
};


struct AttendanceChecklist {
    QDate       month;
    bool        can_write;
    Dbt::Employees              employee;
    QList<Dbt::AttendanceDays>  days;
    Dbt::AttendanceSummary      summary_calculated;
    Dbt::AttendanceSummary      summary_saved;
    static AttendanceChecklist fromMap(const QVariantMap&);
    QVariantMap toMap() const;
    AttendanceChecklist() { can_write = false; }
};


struct AttendancePresent {
    Dbt::Employees  employee;
    QDateTime       date;
    QString         event_type;
    bool            present;
    static AttendancePresent fromMap(const QVariantMap&);
    QVariantMap toMap() const;
};


struct UserEmployeeAccess {
    int     user;
    int     employee;
    bool    can_write;
    bool    can_read;
    UserEmployeeAccess() { user = 0; employee = 0; can_write = false; can_read = false; }
    QVariantMap toMap() const;
};

}

#endif

