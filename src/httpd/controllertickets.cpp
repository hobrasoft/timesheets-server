/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controllertickets.h"
#include "db.h"

using namespace Httpd;


ControllerTickets::ControllerTickets(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
}


void ControllerTickets::serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    serviceOK(request, response, toList(DB->tickets()));
}


