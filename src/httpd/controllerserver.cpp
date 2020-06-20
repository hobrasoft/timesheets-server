/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controllerserver.h"
#include "httprequest.h"
#include "msettings.h"
#include "version.h"
#include "pdebug.h"
#include "pluginstore.h"
#include <QTimer>
#include <QFileInfo>
#include <QDir>

using namespace Httpd;

ControllerServer::ControllerServer(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
}

void ControllerServer::serviceIdGet (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {

    if (id == "about") {
        AUTHORIZEREQUEST(Security::Permissions::Server);

        QVariantMap data;
        data["version"]     = VERSION;
        data["configfile"]  = MSETTINGS->fileName();
        serviceOK(request, response, data);
        return;
        }

    if (id == "configfile") {
        AUTHORIZEREQUEST(Security::Permissions::ServerConfigFile);
        QString configstring;
        QString configfile = MSETTINGS->fileName();
        QFile file(configfile);
        if (file.open(QIODevice::ReadOnly)) {
            configstring = QString::fromUtf8(file.readAll());
            configstring = configstring.replace(QRegExp("password\\s*=[^\\n]*\\n"), "password = ***\n");
            file.close();
            }
        QVariantMap data;
        data["configfile"]  = MSETTINGS->fileName();
        data["config"]      = configstring;
        serviceOK(request, response, data);
        return;
        }

    if (id == "name") {
        AUTHORIZEREQUEST(Security::Permissions::Server);
        QVariantMap data;
        data["name"] = MSETTINGS->serverName();
        serviceOK(request, response, data);
        }

    if (id == "description") {
        AUTHORIZEREQUEST(Security::Permissions::Server);
        QVariantMap data;
        data["description"] = MSETTINGS->serverDescription();
        serviceOK(request, response, data);
        return;
        }

    if (id == "restart") {
        PDEBUG << "restart";
        AUTHORIZEREQUEST(Security::Permissions::ServerRestart);
        QVariantMap data;
        if (request->parameter("reallyrestart") == "YES") {
            // nelze volat přímo, je v jiném vláknu
            // QTimer::singleShot(0, FOTOMONSERVER, SLOT(restart()));
            serviceOK(request, response);
          } else {
            serviceError(request, response, 405, "bad-request", "Could not restart server, missing parameter");
            }
        return;
        }

    if (id == "plugins") {
        AUTHORIZEREQUEST(Security::Permissions::Server);
        QVariantMap data;
        data["plugins"] = PLUGINSTORE->pluginNames();
        serviceOK(request, response, data);
        return;
        }

    if (id == "objects") {
        // serviceOK(request, response, objects(FOTOMONSERVER));
        serviceOK(request, response, QVariantMap());
        }

}


QVariantList ControllerServer::objects(const QObject *object, int depth) {
    QVariantList data;
    if (depth > 10) {
        return data;
        }
    QObjectList list = object->children();
    for (int i=0; i<list.size(); i++) {
        const QObject *x = list[i];
        QVariantMap objectdata;
        objectdata["address"]  = (quint64)x;
        objectdata["name"]     = x->objectName().isEmpty() ? "unnamed" : x->objectName();
        objectdata["class"]    = x->metaObject()->className();
        objectdata["children"] = objects(x, depth+1);
        data << objectdata;
        }
    return data;
}

