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


void DatabasePluginFotomon::createCategoriesTemporaryTable() {
    MSqlQuery q(m_db);

    QString lang = "en";
    q.prepare("select language from users where \"user\" = :user;");
    q.bindValue(":user", m_user);
    q.exec();
    if (q.next()) {
        lang = q.value(0).toString();
        }

    q.exec("create temporary table timesheet_categories "
            "(type int, system int, category int, description text, parent_type int)"
            ";");

    // Vybere všechny typy knih
    q.prepare("insert into timesheet_categories (type, system, category, description, parent_type) "
            " select type, null, null, formal_description->>:lang, 1 "
            " from tickets_types;");
    q.bindValue(":lang", lang);   
    q.exec();

    // Vybere všechny kombinace záznamů typ-system
    q.prepare("insert into timesheet_categories (type, system, category, description, parent_type) "
            " select distinct on (tt.type, s.system) tt.type, s.system, null, s.description, 2 "
            " from tickets_categories_types_systems tt, systems s "
            " where s.show_on_panel and s.show_on_web "
            " and s.system in (select system from users_systems where \"user\" = :user); "
            );
    q.bindValue(":user", m_user);
    q.exec();

    // Vybere všechny kombinace záznamů typ-system-kategorie, kde EXISTUJE záznam typ-system
    q.prepare("insert into timesheet_categories (type, system, category, description, parent_type) "
            " select tt.type, s.system, tc.category, tc.formal_description->>:lang, 3 "
            " from tickets_categories_types_systems tt, systems s, tickets_categories tc "
            " where tt.system = s.system "
            " and tc.category = tt.category "
            " and s.show_on_panel and s.show_on_web "
            " and s.system in (select system from users_systems where \"user\" = :user); "
            );
    q.bindValue(":lang", lang);
    q.bindValue(":user", m_user);
    q.exec();

    // Vybere všechny kombinace záznamů typ-system-kategorie, kde NEEXISTUJE záznam typ-system
    q.prepare("insert into timesheet_categories (type, system, category, description, parent_type) "
            " select tt.type, s.system, tc.category, s.description, 4 "
            " from systems s, tickets_types tt, tickets_categories tc "
            " where s.show_on_panel and s.show_on_web "
            " and s.system in (select system from users_systems where \"user\" = :user) "
            " and not exists (select 1 "
            "       from tickets_categories_types_systems tcts "
            "      where tcts.type = tt.type "
            "        and tcts.system = s.system ); "
            );
    q.bindValue(":user", m_user);
    q.exec();

}


QString DatabasePluginFotomon::categoryKey(const QVariant& type, const QVariant& system, const QVariant& category, int parent_type) {
    if (parent_type == 1) { 
        return QString(R"'({"type":%1})'")
            .arg(type.toInt())
            ;
        }

    if (parent_type == 2) {
        return QString(R"'({"type":%1,"system":%2})'")
            .arg(type.toInt())
            .arg(system.toInt())
            ;
        }

    if (parent_type == 3) {
        return QString(R"'({"type":%1,"system":%2,"category":%3})'")
            .arg(type.toInt())
            .arg(system.toInt())
            .arg(category.toInt())
            ;
        }

    if (parent_type == 4) {
        return QString(R"'({"type":%1,"system":%2})'")
            .arg(type.toInt())
            .arg(system.toInt())
            ;
        }

    return QString();
}


QString DatabasePluginFotomon::parentCategoryKey(const QVariant& type, const QVariant& system, const QVariant& category, int parent_type) {
    if (parent_type == 1) { 
        return QString();
        }

    if (parent_type == 2) {
        return QString(R"'({"type":%1})'")
            .arg(type.toInt())
            ;
        }

    if (parent_type == 3) {
        return QString(R"'({"type":%1,"system":%2})'")
            .arg(type.toInt())
            .arg(system.toInt())
            ;
        }

    if (parent_type == 4) {
        return QString(R"'({"type":%1})'")
            .arg(type.toInt())
            ;
        }

    return QString();
}



QList<Dbt::Categories> DatabasePluginFotomon::categories() {
    createCategoriesTemporaryTable();

    QList<Dbt::Categories> list;
    MSqlQuery q(m_db);

    q.exec("select type, system, category, description, parent_type from timesheet_categories;");
    while (q.next()) {
        Dbt::Categories x;
        QVariant type = q.value(0);
        QVariant system = q.value(1);
        QVariant category = q.value(2);
        QVariant description = q.value(3);
        int parent_type = q.value(4).toInt();

        x.category          =       categoryKey(type, system, category, parent_type);
        x.parent_category   = parentCategoryKey(type, system, category, parent_type);
        x.description = description.toString();
        list << x;
        }

    PDEBUG << "pocet kategorii" << list.size();
    return list;
}



