/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controllerstatuses.h"
#include "db.h"

using namespace Httpd;


ControllerStatuses::ControllerStatuses(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
}


void ControllerStatuses::serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    QString category = request->parameter("category");
    QString status   = request->parameter("previousStatus");
    serviceOK(request, response, toList(db()->statuses(category, previousStatus)));
}


void ControllerStatuses::serviceIdGet(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    auto list = db()->statuses(id);
    if (list.isEmpty()) {
        serviceError(request, response, 404, "not-found", "NotFound");
        return;
        }
    serviceOK(request, response, list[0].toMap());
}

void ControllerStatuses::serviceIdPut(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceOK(request, response, putKey(db()->save(Dbt::Statuses::fromMap(data))));
}


void ControllerStatuses::serviceIdPost(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceIdPut(request, response, data);
}


void ControllerStatuses::serviceIdDelete(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    db()->remove(Dbt::Statuses(id));
    serviceOK(request, response);
}

