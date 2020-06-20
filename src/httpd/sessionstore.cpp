/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "sessionstore.h"
#include "httpsessionstore.h"

using namespace Httpd;

SessionStore *SessionStore::m_sessionStore = NULL;

SessionStore::SessionStore(const HobrasoftHttpd::HttpSettings *settings, QObject *parent) : 
        HobrasoftHttpd::HttpSessionStore(settings, parent) {
    m_sessionStore = this;
}


SessionStore *SessionStore::sessionStore(const HobrasoftHttpd::HttpSettings *settings, QObject *parent) {
    if (m_sessionStore == NULL) {
        new SessionStore(settings, parent);
        }
    return m_sessionStore;
}


