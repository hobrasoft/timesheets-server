/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "sessionstore.h"
#include "httpsessionstore.h"
#include "msettings.h"
#include "json.h"
#include <QFile>
#include <QDateTime>
#include <QVariant>
#include <QVariantMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

using namespace Httpd;

SessionStore *SessionStore::m_sessionStore = NULL;

SessionStore::SessionStore(const HobrasoftHttpd::HttpSettings *settings, QObject *parent) :
        HobrasoftHttpd::HttpSessionStore(settings, parent) {
    m_sessionStore = this;
    load();
}

SessionStore::~SessionStore() {
    save();
}


SessionStore *SessionStore::sessionStore(const HobrasoftHttpd::HttpSettings *settings, QObject *parent) {
    if (m_sessionStore == NULL) {
        new SessionStore(settings, parent);
        }
    return m_sessionStore;
}

void SessionStore::load() {
    QFile file(MSETTINGS->httpdSessionsFile());
    if (!file.open(QIODevice::ReadOnly)) { return; }
    QByteArray json = file.readAll();
    file.close();
    QVariant data = JSON::data(json);
    QVariantList list = data.toList();
    for (const QVariant& v : list) {
        QVariantMap map = v.toMap();
        HobrasoftHttpd::HttpSessionData d;
        d.id = map.value("id").toByteArray();
        d.lastAccess = QDateTime::fromString(map.value("lastAccess").toString(), Qt::ISODate);
        QVariantHash values = map.value("values").toHash();
        QHash<QString, QVariant> hash;
        for (auto it = values.constBegin(); it != values.constEnd(); ++it) {
            hash.insert(it.key(), it.value());
            }
        d.values = hash;
        setSessionData(d);
        }
}

void SessionStore::save() {
    QVariantList list;
    QList<HobrasoftHttpd::HttpSessionData> sessions = sessionsData();
    for (const HobrasoftHttpd::HttpSessionData& d : sessions) {
        QVariantMap map;
        map["id"] = d.id;
        map["lastAccess"] = d.lastAccess.toString(Qt::ISODate);
        QVariantMap values;
        QHashIterator<QString, QVariant> it(d.values);
        while (it.hasNext()) {
            it.next();
            values.insert(it.key(), it.value());
            }
        map["values"] = values;
        list << map;
        }
    QFile file(MSETTINGS->httpdSessionsFile());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) { return; }
    file.write(JSON::json(list));
    file.close();
}


