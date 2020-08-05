/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controllerticketsvw.h"
#include "httprequest.h"
#include "db.h"
#include "pdebug.h"

using namespace Httpd;


ControllerTicketsVw::ControllerTicketsVw(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
}


void ControllerTicketsVw::serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    bool all = QVariant(request->parameter("all")).toBool();
    serviceOK(request, response, toList(db()->ticketsVw(all)));
}


void ControllerTicketsVw::serviceIdGet (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    bool all = QVariant(request->parameter("all")).toBool();
    auto list = db()->ticketsVw(id.toInt(), all);
    if (list.isEmpty()) {
        serviceError(request, response, 404, "not-found", "NotFound");
        return;
        }
    serviceOK(request, response, list[0].toMap());
}

