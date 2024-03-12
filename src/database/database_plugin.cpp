/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */

#include "database_plugin.h"
#include "database.h"
#include "msettings.h"

using namespace Db::Plugins;


DatabasePlugin::DatabasePlugin(QObject *parent) : QObject (parent) {
    Q_ASSERT(parent != nullptr);
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


QList<Dbt::Statuses> DatabasePlugin::statuses(const QString& category, const QStringList& previousStatuses) {
    QList<Dbt::Statuses> values;
    for (int i=0; i<previousStatuses.size(); i++) {
        values += statuses(category, previousStatuses[i]);
        }

    QMap<QString, int> counts;
    for (int i=0; i<values.size(); i++) {
        counts[values[i].status] += 1;
        }

    QList<QString> selected;
    QMapIterator<QString, int> iterator(counts);
    while (iterator.hasNext()) {
        iterator.next();
        const QString& status = iterator.key();
        const int count = iterator.value();
        if (count != previousStatuses.size()) { continue; }
        selected << status;
        }

    QList<Dbt::Statuses> list;
    QList<QString> selected2;
    for (int i=0; i<values.size(); i++) {
        if (selected2.contains(values[i].status)) { continue; }
        if (!selected.contains(values[i].status)) { continue; }
        selected2 << values[i].status;
        list << values[i];
        }
    return list;
}


QList<Dbt::ServerInfo> DatabasePlugin::serverInfo() {
    QList<Dbt::ServerInfo> list;
    Dbt::ServerInfo x;
    x.name = MSETTINGS->serverName();
    x.description = MSETTINGS->serverDescription();
    list << x;
    return list;
}

