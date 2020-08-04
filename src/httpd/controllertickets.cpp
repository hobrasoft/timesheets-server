/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controllertickets.h"
#include "httprequest.h"
#include "db.h"
#include "pdebug.h"

using namespace Httpd;


ControllerTickets::ControllerTickets(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
}


void ControllerTickets::serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    bool all = QVariant(request->parameter("all")).toBool();
    serviceOK(request, response, toList(db()->tickets(all)));
}


void ControllerTickets::serviceIdGet (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    bool all = QVariant(request->parameter("all")).toBool();
    auto list = db()->tickets(id.toInt(), all);
    if (list.isEmpty()) {
        serviceError(request, response, 404, "not-found", "NotFound");
        return;
        }
    serviceOK(request, response, list[0].toMap());
}

