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


int DatabasePluginFotomon::m_user = 0; 

DatabasePluginFotomon::~DatabasePluginFotomon() {
    close();
}


DatabasePluginFotomon::DatabasePluginFotomon(QObject *parent) : Db::Plugins::DatabasePlugin(parent) {
    Q_ASSERT(parent != NULL);
    setObjectName("DatabasePluginFotomon");
}


bool DatabasePluginFotomon::open() {
    m_lang = "uk";
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
    m_user = 0;
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
        m_user = x.user;
        list << x;
        }
    return list;
}


QList<Dbt::Categories> DatabasePluginFotomon::categories() {
    PDEBUG;

    QList<Dbt::Categories> list;
    MSqlQuery q(m_db);

    q.prepare("select type, abbreviation, formal_description->>:lang from tickets_types;");
    q.bindValue(":lang", m_lang);   
    q.exec();
    PDEBUG << q.lastBoundQuery();
    while (q.next()) {
        int i=0;
        Dbt::Categories x;
        x.category = QString("{type:%1}").arg(q.value(i++).toInt());
        x.abbreviation = q.value(i++).toString();
        x.description = q.value(i++).toString();
        list << x;
        }


    q.prepare("select distinct on (tt.type, s.system) tt.type, s.system, s.description "
            " from tickets_categories_types_systems tt, systems s "
            " where tt.system = s.system "
            " and s.show_on_panel and s.show_on_web "
            " and s.system in (select system from users_systems where \"user\" = :user); "
            );
    q.bindValue(":user", m_user);
    q.exec();
    while (q.next()) {
        Dbt::Categories x;
        x.category = QString(R"'({"type":%1,"system":%2})'")
                .arg(q.value(0).toInt())
                .arg(q.value(1).toInt())
                ;
        x.parent_category = QString(R"'({"type":%1}")'").arg(q.value(0).toInt());
        x.description = q.value(2).toString();
        list << x;
        }

    q.prepare("select tt.type, s.system, tc.category, tc.formal_description->>:lang "
            " from tickets_categories_types_systems tt, systems s, tickets_categories tc "
            " where tt.system = s.system "
            " and tc.category = tt.category "
            " and s.show_on_panel and s.show_on_web "
            " and s.system in (select system from users_systems where \"user\" = :user); "
            );
    q.bindValue(":lang", m_lang);
    q.bindValue(":user", m_user);
    q.exec();
    while (q.next()) {
        Dbt::Categories x;
        x.category = QString(R"'({"type":%1,"system":%2,"category":%3})'")
                .arg(q.value(0).toInt())
                .arg(q.value(1).toInt())
                .arg(q.value(2).toInt())
                ;
        x.parent_category = QString(R"'({"type":%1,"system":%2})'")
                .arg(q.value(0).toInt())
                .arg(q.value(1).toInt())
                ;
        x.description = q.value(3).toString();
        list << x;
        }


    PDEBUG << "pocet kategorii" << list.size();
    return list;
}



