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
    serviceOK(request, response, toList(db()->statuses()));
}


void ControllerStatuses::serviceIdPut(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    db()->save(Dbt::Statuses::fromMap(data));
}


void ControllerStatuses::serviceIdPost(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceIdPut(request, response, data);
}


void ControllerStatuses::serviceIdDelete(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    db()->remove(Dbt::Statuses(id));
    serviceOK(request, response);
}

