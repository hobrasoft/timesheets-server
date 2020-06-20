/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "httpd.h"
#include "msettings.h"
#include "requestmapper.h"
#include "sessionstore.h"
#include "pluginstore.h"
#include "httpserver.h"
#include "pdebug.h"
#include <QUuid>


Httpd::Httpd *Httpd::Httpd::m_httpd = NULL;

Httpd::Httpd::Httpd(const HobrasoftHttpd::HttpSettings *settings, QObject *parent) : HobrasoftHttpd::HttpServer(settings, parent) {
    m_httpd = this;
    /**
     * Vytvoří SessionStore pro ukládání session pro přihlašování
     */
    SessionStore::sessionStore(settings, this);
    SessionStore::sessionStore()->setSessionCookieName(
        QUuid::createUuid().toString().replace("{","N").replace("}","").replace("-","_")
        );
    PluginStore::pluginStore(this);
}


Httpd::Httpd *Httpd::Httpd::httpd(QObject *parent) {
    if (m_httpd == NULL) {
        if (parent == NULL) {
            qFatal("Httpd::httpd(NULL)");
            }
        HobrasoftHttpd::HttpSettings *settings = new HobrasoftHttpd::HttpSettings(MSETTINGS, "httpd", parent);
        settings->setDefaultThreads(true);
        new Httpd(settings, parent);
        }
    return m_httpd;
}


HobrasoftHttpd::HttpRequestHandler *Httpd::Httpd::requestHandler(HobrasoftHttpd::HttpConnection *connection) {
    // PDEBUG << "Httpd::Httpd::requestHandler()";
    return new RequestMapper(connection);
}

