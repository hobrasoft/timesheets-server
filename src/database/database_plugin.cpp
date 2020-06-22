/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */

#include "database_plugin.h"

using namespace Db::Plugins;


DatabasePlugin::DatabasePlugin(QObject *parent) : QObject (parent) {
    Q_ASSERT(parent != NULL);
    setObjectName("DatabasePlugin-Fotomon");
}


QString DatabasePlugin::connectionName() const {
    return m_db.connectionName();
}


