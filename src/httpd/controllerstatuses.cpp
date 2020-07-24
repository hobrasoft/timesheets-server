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


