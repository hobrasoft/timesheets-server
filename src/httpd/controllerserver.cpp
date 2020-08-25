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
#include "timesheetserver.h"
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
        data["configfile"]  = MSETTINGS->fileName();
        data["name"]        = MSETTINGS->serverName();
        data["description"] = MSETTINGS->serverDescription();
        data["git_commit"]  = GIT_COMMIT;
        data["git_branch"]  = GIT_BRANCH;
        data["version"]     = VERSION;
        data["qt_version"]  = qtVersion();
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

    if (id == "restart") {
        // AUTHORIZEREQUEST(Security::Permissions::ServerRestart);
        QVariantMap data;
        PDEBUG << "restart :-)))";
        if (request->parameter("reallyrestart") == "YES" && MSETTINGS->serverEnableRemoteRestart()) {
            // nelze volat přímo, je v jiném vláknu
            QTimer::singleShot(100, TimesheetServer::instance(), &TimesheetServer::restart);
            QVariantMap data;
            data["ok"] = true;
            PDEBUG << "restarting";
            serviceOK(request, response, data);
            return;
          } else {
            PDEBUG << "restart disabled";
            serviceError(request, response, 405, "bad-request", "Could not restart server");
            return;
            }
        PDEBUG << "a doprdele....";
        return;
        }

    if (id == "objects") {
        serviceOK(request, response, objects(TimesheetServer::instance()));
        return;
        }

    serviceError(request, response, 404, "not-found", "Not found");

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

