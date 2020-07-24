/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */

#include "database.h"
#include "database_plugin.h"
#include "database_plugin_fotomon.h"
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


QList<Dbt::Users> Database::authenticate(const QString& user, const QString& password) {
    return m_dbplugin->authenticate(user,password);
}


QList<Dbt::Categories> Database::categories() {
    return m_dbplugin->categories();
}


QList<Dbt::StatusOrder> Database::statusOrder() {
    return m_dbplugin->statusOrder();
}


QList<Dbt::Statuses> Database::statuses() {
    return m_dbplugin->statuses();
}


QList<Dbt::Tickets> Database::tickets() {
    return m_dbplugin->tickets();
}

