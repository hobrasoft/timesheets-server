/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "database_plugin_fotomon.h"
#include "msettings.h"
#include "msqlquery.h"
#include "pdebug.h"
#include <QUuid>
#include <QSqlError>
#include <QCryptographicHash>

using namespace Db::Plugins;


DatabasePluginFotomon::~DatabasePluginFotomon() {
    close();
}


DatabasePluginFotomon::DatabasePluginFotomon(QObject *parent) : Db::Plugins::DatabasePlugin(parent) {
    Q_ASSERT(parent != NULL);
    setObjectName("DatabasePluginFotomon");
}


bool DatabasePluginFotomon::open() {
    m_db = QSqlDatabase::addDatabase("QPSQL", QUuid::createUuid().toString().toUtf8());

    m_db.setDatabaseName ( m_databasename );
    m_db.setHostName     ( m_hostname     );
    m_db.setPort         ( m_port         );
    m_db.setUserName     ( m_username     );
    m_db.setPassword     ( m_password     );

    if (!m_db.open()) {
        PDEBUG << "Cannot connect to database Fotomon";
        PDEBUG << m_db.lastError().text();
        return false;
        }

    return true;
}


bool DatabasePluginFotomon::close() {
    m_db.close();
    return true;
}


void DatabasePluginFotomon::begin() {
    MSqlQuery q(m_db);
    q.exec("begin;");
}


void DatabasePluginFotomon::commit() {
    MSqlQuery q(m_db);
    q.exec("commit;");
}

  
QList<Dbt::Users> DatabasePluginFotomon::authenticate(const QString& login, const QString& password) {
    QString md5 = QString::fromUtf8(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex());
    QList<Dbt::Users> list;
    MSqlQuery q(m_db);
    q.prepare("select \"user\", login, name from users where login = :login and password = :password and is_active and not is_deleted;");
    q.bindValue(":login", login);
    q.bindValue(":password", md5);
    q.exec();
    while (q.next()) {
        int i=0;
        PDEBUG << q.value(0).toString();
        Dbt::Users x;
        x.user          = q.value(i++).toInt();
        x.login         = q.value(i++).toString();
        x.name          = q.value(i++).toString();
        list << x;
        }
    return list;
}

