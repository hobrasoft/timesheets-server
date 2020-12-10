/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controllerstatustemplates.h"
#include "httprequest.h"
#include "db.h"

using namespace Httpd;


ControllerStatusTemplates::ControllerStatusTemplates(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
}


void ControllerStatusTemplates::serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    serviceOK(request, response, toList(db()->statusTemplates()));
}


void ControllerStatusTemplates::serviceIdGet(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    auto list = db()->statusTemplates(id.toInt());
    if (list.isEmpty()) {
        serviceError(request, response, 404, "not-found", "NotFound");
        return;
        }
    serviceOK(request, response, list[0].toMap());
}


