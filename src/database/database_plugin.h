/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _DatabasePlugin_H_
#define _DatabasePlugin_H_

#include <QSqlDatabase>
#include <QObject>
#include <QStringList>
#include <QDate>
#include "dbt.h"
#include "authenticateduser.h"

namespace Db {
namespace Plugins {

/**
 * @brief
 */
class DatabasePlugin : public QObject {
    Q_OBJECT
  public:
    DatabasePlugin(QObject *);

    void setAuthenticatedUser(const AuthenticatedUser *x) { m_authenticatedUser = x; }

    virtual void setDatabaseName(const QString&) = 0;
    virtual void setHostname(const QString&) = 0;
    virtual void setPort(int) = 0;
    virtual void setUserName(const QString&) = 0;
    virtual void setPassword(const QString&) = 0;
  
    virtual bool open() = 0;
    virtual bool close() = 0;
    virtual void begin() = 0;
    virtual void commit() = 0;
    virtual QString connectionName() const;

    virtual void                                    changePassword(const QString& login, const QString& oldpassword, const QString& newpassword) = 0;
    virtual QList<Dbt::Users>                       authenticate(const QString& user, const QString& password) = 0;
    virtual QList<Dbt::Users>                       users(int id) = 0;
    virtual QList<Dbt::Categories>                  categories(const QString& id) = 0;
    virtual QList<Dbt::Categories>                  categoriesToRoot(const QString& id) = 0;
    virtual QList<Dbt::Categories>                  subcategories(const QString& parent) = 0;
    virtual QList<Dbt::Categories>                  siblingcategories(const QString& parent) = 0;
    virtual QList<Dbt::UsersCategories>             usersCategories(int id, int user, const QString& category) = 0;
    virtual QList<Dbt::CategoriesOverview>          categoriesOverview(const QStringList& statuses) = 0;
    virtual QList<Dbt::StatusOrder>                 statusOrder(const QString& id) = 0;
    virtual QList<Dbt::Statuses>                    statuses(const QString& id) = 0;
    virtual QList<Dbt::Statuses>                    statuses(const QString& category, const QString& prevstatus) = 0;
    virtual QList<Dbt::Statuses>                    statuses(const QString& category, const QStringList& prevstatuses);

    virtual QList<Dbt::StatusTemplates>             statusTemplates(int id) = 0;

    virtual QList<Dbt::Tickets>                     tickets(bool all) = 0;
    virtual QList<Dbt::Tickets>                     tickets(int ticket, bool all) = 0;
    virtual QList<Dbt::Tickets>                     tickets(const Dbt::Categories&, bool all) = 0;

    virtual QList<Dbt::TicketsVw>                   ticketsVw(bool all) = 0;
    virtual QList<Dbt::TicketsVw>                   ticketsVw(int ticket, bool all) = 0;
    virtual QList<Dbt::TicketsVw>                   ticketsVw(const Dbt::Categories&, bool all) = 0;

    virtual QList<Dbt::TicketStatus>                ticketStatus(int id) = 0;
    virtual QList<Dbt::TicketStatus>                ticketStatus(bool all) = 0;
    virtual QList<Dbt::TicketStatus>                ticketStatus(int ticket, bool all) = 0;

    virtual QList<Dbt::TicketValues>                ticketValues(int id) = 0;
    virtual QList<Dbt::TicketValues>                ticketValues(bool all) = 0;
    virtual QList<Dbt::TicketValues>                ticketValues(int ticket, bool all) = 0;

    virtual QList<Dbt::TicketFiles>                 ticketFiles(int id) = 0;
    virtual QList<Dbt::TicketFiles>                 ticketFiles(bool all) = 0;
    virtual QList<Dbt::TicketFiles>                 ticketFiles(int ticket, bool all) = 0;

    virtual QList<Dbt::TicketTimesheets>            ticketTimesheets(int id) = 0;
    virtual QList<Dbt::TicketTimesheets>            ticketTimesheets(bool all) = 0;
    virtual QList<Dbt::TicketTimesheets>            ticketTimesheets(int ticket, bool all) = 0;
    virtual QList<Dbt::TicketTimesheets>            runningTimesheets(int ticket) = 0;
    virtual QList<Dbt::TicketTimesheets>            startTimesheet(int ticket) = 0;
    virtual QList<Dbt::TicketTimesheets>            stopTimesheet(int ticket) = 0;
    virtual QList<Dbt::TicketTimesheets>            toggleTimesheet(int ticket) = 0;

    virtual QList<Dbt::ClientSettings>              clientSettings() = 0;
    virtual QList<Dbt::ServerInfo>                  serverInfo();
    virtual QList<Dbt::Overview>                    overview(const QString& category, const QStringList& statuses) = 0;
    virtual QList<Dbt::Overview>                    overview(const QString& overviewId) = 0;
    virtual QList<Dbt::OverviewList>                overviewList() = 0;

    virtual QList<Dbt::Departments>                 departments(int department) { Q_UNUSED(department); return QList<Dbt::Departments>(); }
    virtual QList<Dbt::Doors>                       doors(int door) { Q_UNUSED(door); return QList<Dbt::Doors>(); }
    virtual QList<Dbt::Employees>                   employees(int employee) { Q_UNUSED(employee); return QList<Dbt::Employees>(); }
    virtual QList<Dbt::EventTypes>                  eventTypes(const QString& eventType) { Q_UNUSED(eventType); return QList<Dbt::EventTypes>(); }
    virtual QList<Dbt::Events>                      events(int event,
                                                         int employee = -1,
                                                         const QDate& month = QDate(),
                                                         int limit = -1,
                                                         int offset = -1)
        { Q_UNUSED(event); Q_UNUSED(employee); Q_UNUSED(month); Q_UNUSED(limit); Q_UNUSED(offset); return QList<Dbt::Events>(); }
    virtual QList<Dbt::DepartmentHasManager>        departmentHasManager(const Dbt::DepartmentHasManager&) { return QList<Dbt::DepartmentHasManager>(); }
    virtual QList<Dbt::DepartmentHasMember>         departmentHasMember(const Dbt::DepartmentHasMember&) { return QList<Dbt::DepartmentHasMember>(); }
    virtual QList<Dbt::EmployeeCanOpenDoor>         employeeCanOpenDoor(const Dbt::EmployeeCanOpenDoor&) { return QList<Dbt::EmployeeCanOpenDoor>(); }
    virtual QList<Dbt::EmployeeHasRfid>             employeeHasRfid(const Dbt::EmployeeHasRfid&) { return QList<Dbt::EmployeeHasRfid>(); }
    virtual QList<Dbt::Rfids>                       rfids(int rfid) = 0;
    virtual QList<Dbt::Holidays>                    holidays(const QDate& date) { Q_UNUSED(date); return QList<Dbt::Holidays>(); }
    virtual QList<Dbt::WorkCalendar>                workCalendar(const QDate& period) { Q_UNUSED(period); return QList<Dbt::WorkCalendar>(); }
    virtual void                                    generateWorkCalendar(int year) { Q_UNUSED(year); }
    virtual void                                    copyHolidays(int fromYear, int toYear) { Q_UNUSED(fromYear); Q_UNUSED(toYear); }
    virtual QList<Dbt::AttendanceChecklist>         attendanceChecklist(int employee, const QDate& month) { Q_UNUSED(employee); Q_UNUSED(month); return QList<Dbt::AttendanceChecklist>(); }
    virtual QList<Dbt::AttendanceSummary>           attendanceSummary(int employee, const QDate& month) { Q_UNUSED(employee); Q_UNUSED(month); return QList<Dbt::AttendanceSummary>(); }
    virtual QList<Dbt::AttendancePresent>           attendancePresent() { return QList<Dbt::AttendancePresent>(); }

    virtual void remove(const Dbt::Users& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::UsersCategories& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::Categories& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::StatusOrder& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::Statuses& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::Tickets& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::TicketStatus& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::TicketValues& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::TicketFiles& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::TicketTimesheets& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::OverviewList& data) { Q_UNUSED(data); }

    virtual void remove(const Dbt::Departments& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::Doors& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::Employees& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::EventTypes& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::Events& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::DepartmentHasManager& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::DepartmentHasMember& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::EmployeeCanOpenDoor& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::EmployeeHasRfid& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::Rfids& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::Holidays& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::WorkCalendar& data) { Q_UNUSED(data); }
    virtual void remove(const Dbt::AttendanceSummary& data) { Q_UNUSED(data); }

    virtual QVariant save(const Dbt::Users& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::UsersCategories& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::Categories& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::StatusOrder& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::Statuses& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::Tickets& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::TicketsVw& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::TicketStatus& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::TicketValues& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::TicketFiles& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::TicketTimesheets& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::ServerInfo& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::AppendStatuses& data) { Q_UNUSED(data); return QVariant(); }

    virtual QVariant save(const Dbt::Departments& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::Doors& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::Employees& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::EventTypes& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::Events& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::DepartmentHasManager& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::DepartmentHasMember& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::EmployeeCanOpenDoor& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::EmployeeHasRfid& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::Rfids& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::Holidays& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::WorkCalendar& data) { Q_UNUSED(data); return QVariant(); }
    virtual QVariant save(const Dbt::AttendanceSummary& data) { Q_UNUSED(data); return QVariant(); }

  protected:
    int     userId() const;
    bool    userAuthenticated() const;
    QString userLogin() const;
    QString userLang() const;

  protected:
    // virtual QVariant    lastInsertRowId(const QString& sequence_name = QString(), Db::MSqlQuery *q = NULL) = 0;
    // virtual QString     formatLimitOffset(int limit, int offset) const = 0;
    QSqlDatabase m_db;

  public:
    const AuthenticatedUser *m_authenticatedUser = nullptr;


};

}
}

#endif

