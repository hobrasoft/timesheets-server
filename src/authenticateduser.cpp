/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "authenticateduser.h"
#include "db.h"

AuthenticatedUser::AuthenticatedUser(QObject *parent) : QObject(parent) {
    m_authenticated = false;
    m_user = 0;
}


bool AuthenticatedUser::authenticate(const QString& login, const QString& password) {
    m_login = login;
    m_name.clear();
    m_lang = "en";
    m_authenticated = false;
    Db::Database *db = Db::Database::create(this);
    QList<Dbt::Users> list = db->authenticate(login, password);
    if (list.isEmpty()) {
        return false;
        }
    const Dbt::Users& item = list[0];
    m_user = item.user;
    m_name = item.name;
    m_lang = item.lang;
    m_authenticated = true;
    return true;
}


