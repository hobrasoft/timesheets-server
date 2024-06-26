/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controllerusers.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "json.h"
#include "pdebug.h"
#include "db.h"

using namespace Httpd;


ControllerUsers::ControllerUsers(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
}


void ControllerUsers::serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    serviceOK(request, response, toList(db()->users()));
}


void ControllerUsers::serviceIdGet(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    PDEBUG;
    auto list = db()->users(id.toInt());
    if (list.isEmpty()) {
        serviceError(request, response, 404, "not-found", "NotFound");
        return;
        }
    serviceOK(request, response, list[0].toMap());
}


void ControllerUsers::service(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    QString path = request->path();
    QString method = request->method();

    if ((method == "PUT" || method == "POST") && path.endsWith("/set-password")) {
        bool ok;
        QVariantMap data = JSON::data(request->body(), &ok).toMap();
        if (!ok) {
            serviceError(request, response, 400, "bad-request", "Could not parse JSON data");
            return;
            }
        QString login       = data["login"].toString();
        QString oldpassword = data["oldpassword"].toString();
        QString newpassword = data["newpassword"].toString();
        db()->changePassword(login, oldpassword, newpassword);
        serviceOK(request, response);
        return;
        }

    AbstractController::service(request, response);
}


void ControllerUsers::serviceIdPut(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceOK(request, response, putKey(db()->save(Dbt::Users::fromMap(data))));
}


void ControllerUsers::serviceIdPost(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceIdPut(request, response, data);
}


void ControllerUsers::serviceIdDelete(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    db()->remove(Dbt::Users(id.toInt()));
    serviceOK(request, response);
}

