/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */

#include "database.h"
#include "database_plugin.h"
#include "database_plugin_fotomon.h"
#include "database_plugin_postgres.h"
#include "msettings.h"
#include "pdebug.h"

using namespace Db;

Database *Database::m_instance = nullptr;


Database::~Database() {
    m_instance = nullptr;
}


Database::Database(QObject *parent) : QObject(parent) {
    if (m_instance == nullptr) {
        m_instance = this;
        }

    m_isOpen = false;
    m_dbplugin = nullptr;;
    setObjectName(QString("Database %1").arg((quint64)QThread::currentThread()));
    m_isOpen = open();

}


bool Database::open() {
    QString plugin = MSETTINGS->dbPlugin();

    if (m_isOpen) {
        close();
        }

    if (m_dbplugin != nullptr) {
        m_dbplugin->deleteLater();
        }

    if (plugin == "fotomon") {
        m_dbplugin = new Db::Plugins::DatabasePluginFotomon(this);
        }

    if (plugin == "postgres") {
        m_dbplugin = new Db::Plugins::DatabasePluginPostgres(this);
        }

    if (m_dbplugin == nullptr) {
        PDEBUG << "m_dbplugin == nullptr";
        emit opened(false);
        return false;
        }

    m_dbplugin->setDatabaseName ( MSETTINGS->dbName() );
    m_dbplugin->setHostname     ( MSETTINGS->dbServer() );
    m_dbplugin->setPort         ( MSETTINGS->dbPort() );
    m_dbplugin->setUserName     ( MSETTINGS->dbUser() );
    m_dbplugin->setPassword     ( MSETTINGS->dbPassword() );
    bool rc = m_dbplugin->open();
    m_isOpen = rc;
    emit opened(rc);
    return rc;
}


Database *Database::instance(QObject *parent) {
    if (m_instance == nullptr) {
        Q_ASSERT(parent != nullptr);
        new Database(parent);
        return m_instance;
        }

    return m_instance;
}


Database *Database::create(QObject *parent) {
    Database *db = new Database(parent);
    return db;
}


Database *Database::create(QObject *parent, const AuthenticatedUser *user) {
    Database *db = new Database(parent);
    db->m_dbplugin->setAuthenticatedUser(user);
    return db;
}


void Database::close() {
    if (m_dbplugin != nullptr) {
        m_dbplugin->close();
        }
    m_isOpen = false;
}


QString Database::connectionName() const {
    return m_dbplugin->connectionName();
}


void Database::begin() {
    return m_dbplugin->begin();
}


void Database::commit() {
    return m_dbplugin->commit();
}


void Database::changePassword(const QString& login, const QString& oldpassword, const QString& newpassword) {
    m_dbplugin->changePassword(login, oldpassword, newpassword);
}


QList<Dbt::Users> Database::authenticate(const QString& user, const QString& password) {
    return m_dbplugin->authenticate(user,password);
}


QList<Dbt::Users> Database::users(int id){
    return m_dbplugin->users(id);
}


QList<Dbt::UsersCategories> Database::usersCategories(int id, int user, const QString& category){
    return m_dbplugin->usersCategories(id, user, category);
}


QList<Dbt::Categories> Database::categories(const QString& id) {
    return m_dbplugin->categories(id);
}


QList<Dbt::Categories> Database::subcategories(const QString& id) {
    return m_dbplugin->subcategories(id);
}


QList<Dbt::Categories> Database::siblingcategories(const QString& id) {
    return m_dbplugin->siblingcategories(id);
}


QList<Dbt::Categories> Database::categoriesToRoot(const QString& id) {
    return m_dbplugin->categoriesToRoot(id);
}


QList<Dbt::Statuses> Database::statuses(const QString& id) {
    return m_dbplugin->statuses(id);
}


QList<Dbt::Statuses> Database::statuses(const QString& category, const QString& prevStatus) {
    return m_dbplugin->statuses(category, prevStatus);
}


QList<Dbt::Statuses> Database::statuses(const QString& category, const QStringList& prevStatuses) {
    return m_dbplugin->statuses(category, prevStatuses);
}


QList<Dbt::StatusTemplates> Database::statusTemplates(int id) {
    return m_dbplugin->statusTemplates(id);
}


QList<Dbt::StatusOrder> Database::statusOrder(const QString& id) {
    return m_dbplugin->statusOrder(id);
}


QList<Dbt::Tickets> Database::tickets(int ticket, bool all) {
    return m_dbplugin->tickets(ticket, all);
}


QList<Dbt::Tickets> Database::tickets(bool all) {
    return m_dbplugin->tickets(all);
}


QList<Dbt::Tickets> Database::tickets(const Dbt::Categories& category, bool all) {
    return m_dbplugin->tickets(category, all);
}


QList<Dbt::TicketsVw> Database::ticketsVw(int ticket, bool all) {
    return m_dbplugin->ticketsVw(ticket, all);
}


QList<Dbt::TicketsVw> Database::ticketsVw(bool all) {
    return m_dbplugin->ticketsVw(all);
}


QList<Dbt::TicketsVw> Database::ticketsVw(const Dbt::Categories& category, bool all) {
    return m_dbplugin->ticketsVw(category, all);
}


QList<Dbt::TicketStatus> Database::ticketStatus(int ticket, bool all) {
    return m_dbplugin->ticketStatus(ticket, all);
}


QList<Dbt::TicketStatus> Database::ticketStatus(int id) {
    return m_dbplugin->ticketStatus(id);
}


QList<Dbt::TicketStatus> Database::ticketStatus(bool all) {
    return m_dbplugin->ticketStatus(all);
}


QList<Dbt::TicketValues> Database::ticketValues(int id) {
    return m_dbplugin->ticketValues(id);
}


QList<Dbt::TicketValues> Database::ticketValues(bool all) {
    return m_dbplugin->ticketValues(all);
}


QList<Dbt::TicketValues> Database::ticketValues(int ticket, bool all) {
    return m_dbplugin->ticketValues(ticket, all);
}


QList<Dbt::TicketFiles> Database::ticketFiles(int id) {
    return m_dbplugin->ticketFiles(id);
}


QList<Dbt::TicketFiles> Database::ticketFiles(bool all) {
    return m_dbplugin->ticketFiles(all);
}


QList<Dbt::TicketFiles> Database::ticketFiles(int ticket, bool all) {
    return m_dbplugin->ticketFiles(ticket, all);
}


QList<Dbt::TicketTimesheets> Database::ticketTimesheets(int id) {
    return m_dbplugin->ticketTimesheets(id);
}


QList<Dbt::TicketTimesheets> Database::ticketTimesheets(bool all) {
    return m_dbplugin->ticketTimesheets(all);
}


QList<Dbt::TicketTimesheets> Database::ticketTimesheets(int ticket, bool all) {
    return m_dbplugin->ticketTimesheets(ticket, all);
}


QList<Dbt::TicketTimesheets> Database::runningTimesheets(int ticket) {
    return m_dbplugin->runningTimesheets(ticket);
}


QList<Dbt::TicketTimesheets> Database::startTimesheet(int ticket) {
    return m_dbplugin->startTimesheet(ticket);
}


QList<Dbt::TicketTimesheets> Database::stopTimesheet(int ticket) {
    return m_dbplugin->stopTimesheet(ticket);
}


QList<Dbt::TicketTimesheets> Database::toggleTimesheet(int ticket) {
    return m_dbplugin->toggleTimesheet(ticket);
}


QList<Dbt::ClientSettings> Database::clientSettings() {
    return m_dbplugin->clientSettings();
}


QList<Dbt::ServerInfo> Database::serverInfo() {
    return m_dbplugin->serverInfo();
}


QList<Dbt::Overview> Database::overview(const QString& category, const QStringList& statuses) {
    return m_dbplugin->overview(category, statuses);
}


QList<Dbt::Overview> Database::overview(const QString& overviewId) {
    return m_dbplugin->overview(overviewId);
}


QList<Dbt::OverviewList> Database::overviewList() {
    return m_dbplugin->overviewList();
}

