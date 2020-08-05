/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controllerusers.h"
#include "pdebug.h"
#include "db.h"

using namespace Httpd;


ControllerUsers::ControllerUsers(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
}


void ControllerUsers::serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    serviceOK(request, response, toList(db()->users()));
}


void ControllerUsers::serviceIdGet(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    auto list = db()->users(id.toInt());
    if (list.isEmpty()) {
        serviceError(request, response, 404, "not-found", "NotFound");
        return;
        }
    serviceOK(request, response, list[0].toMap());
}


void ControllerUsers::serviceIdPut(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    QVariantMap x;
    x["key"] = db()->save(Dbt::Users::fromMap(data));
    PDEBUG << "USER" << x["key"];
    serviceOK(request, response, x);
}


void ControllerUsers::serviceIdPost(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceIdPut(request, response, data);
}


void ControllerUsers::serviceIdDelete(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    db()->remove(Dbt::Users(id.toInt()));
    serviceOK(request, response);
}

