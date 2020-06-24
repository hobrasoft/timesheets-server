/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "requestmapper.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "httpconnection.h"
#include "sessionstore.h"
#include "staticfilecontroller.h"
#include "msettings.h"
#include "pdebug.h"
#include "pluginstore.h"
#include "plugininterface.h"
#include "requestauthorizer.h"

// nové API
#include "controllerserver.h"
#include "controllercategories.h"
#include "controllerstatusorder.h"
#include "controllerstatuses.h"

using namespace HobrasoftHttpd;
using namespace Httpd;

/**
 * @addtogroup konfigurace
 * @{
 * Konfigurace Httpd::Httpd
 * ------------
 *
 * - __httpd/user__ - uživatelské jméno (admin)
 * - __httpd/password__ - heslo (admin)
 * - __httpd/authorize__ - pokud je true, přístup na stránky bude vyžadovat heslo (true)
 * @}
 */


RequestMapper::~RequestMapper() {
}


RequestMapper::RequestMapper(HobrasoftHttpd::HttpConnection *parent) : HobrasoftHttpd::HttpRequestHandler(parent) {
    // PDEBUG << parent->thread() << this->thread();
    m_authorizer = new RequestAuthorizer(parent);
}


void RequestMapper::service(HttpRequest *request, HttpResponse *response) {
    m_path = request->path();
    PDEBUG << request->fullPath();

    #define ROUTER(adresa, trida) \
        if (m_path.startsWith(adresa)) { \
            AbstractController *controller = new trida (connection()); \
            controller->setAuthorizer(m_authorizer); \
            controller->service(request, response); \
            return; \
            }

    // Možné bez přihlášení
    ROUTER("/api/v1/server/about", ControllerServer);

    // Kontrola přihlášení
    if (!m_authorizer->isLoggedIn(request, response)) {
        return;
        }

    QList<PluginInterface *> plugins = PLUGINSTORE->plugins();
    for (int i=0; i<plugins.size(); i++) {
        PluginInterface *plugin = plugins[i];
        if (!plugin->path().isEmpty() && m_path.startsWith(plugin->path())) {
            HttpRequestHandler *controller = plugin->createController(connection());
            if (controller != NULL) {
                controller->service(request, response);
                return;
                }
            }
        }

    /**
     * Po přihlášení zpracovává ostatní speciální požadavky (event streamy).
     */
    ROUTER("/api/v1/categories",                       ControllerCategories);
    ROUTER("/api/v1/statusorder",                      ControllerStatusOrder);
    ROUTER("/api/v1/statuses",                         ControllerStatuses);
    ROUTER("/api/v1/server",                           ControllerServer);

    /**
     * Po speciálních požadavcích se volá SHTML interpreter.
     * Nevyužívá se shtml default controller, je napsaný vlastní kvůli menu.
     */
    if (m_path.endsWith(".shtml")) {
        QByteArray data;
        try {
            data = readFile(m_path, response);
            } 
        catch (...) {
            return;
            }
        response->setHeader("Content-Type", "text/html; charset=" + connection()->settings()->encoding());
        response->write(data);
        response->flush();
        return;
        }

    /**
     * Nakonec se obslouží statické soubory
     */
    StaticFileController(connection()).service(request, response);
    response->flush();

}


QByteArray RequestMapper::readFile(const QString& path, HttpResponse *response) {
    QByteArray data;
    if (path.startsWith("/..") || path.startsWith("..")) {
        response->setStatus(403,"Forbidden");
        response->write(QString("403 Forbidden: %1<br>\nDo not use ../ in your file path").arg(path).toUtf8());
        response->flush();
        throw false;;
        }

    QFile file(connection()->settings()->docroot()+"/"+path);
    if (!file.exists()) {
        response->setStatus(404, "Not found");
        response->write(QString("404 File not found: %1").arg(file.fileName()).toUtf8());
        response->flush();
        throw false;;
        }

    if (!file.open(QIODevice::ReadOnly)) {
        response->setStatus(403, "Forbidden");
        response->write(QString("403 Forbidden: %1").arg(file.fileName()).toUtf8());
        response->flush();
        throw false;;
        }

    QString rolename = Security::Roles::toString(m_authorizer->role());

    while (!file.atEnd()) {
        QString line = QString::fromUtf8(file.readLine());
        line = line.replace("${ROLE}", rolename);
        if (line.contains(QRegExp("^\\s*<!--\\s*#include\\s+\".+\"\\s*-->\\s*$"))) {
            QStringList lineparts = line.split('"');
            if (lineparts.size() != 3) {
                continue;
                }
            data += readFile(lineparts[1], response);
            continue;
            }
        data += line.toUtf8();
        }

    file.close();
    return data;
}



