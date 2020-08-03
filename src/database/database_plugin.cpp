/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */

#include "database_plugin.h"
#include "database.h"

using namespace Db::Plugins;


DatabasePlugin::DatabasePlugin(QObject *parent) : QObject (parent) {
    Q_ASSERT(parent != NULL);
    setObjectName("DatabasePlugin-Fotomon");
}


QString DatabasePlugin::connectionName() const {
    return m_db.connectionName();
}



int DatabasePlugin::userId() const {
    return (m_authenticatedUser != nullptr) ? m_authenticatedUser->user() : 0;
}


bool DatabasePlugin::userAuthenticated() const {
    return (m_authenticatedUser != nullptr) ? m_authenticatedUser->authenticated() : false;
}

QString DatabasePlugin::userLogin() const {
    return (m_authenticatedUser != nullptr) ? m_authenticatedUser->login() : QString();
}


QString DatabasePlugin::userLang() const {
    return (m_authenticatedUser != nullptr) ? m_authenticatedUser->lang() : QString();
}




