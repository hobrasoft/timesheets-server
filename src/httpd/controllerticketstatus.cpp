/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controllerticketstatus.h"
#include "db.h"

using namespace Httpd;


ControllerTicketStatus::ControllerTicketStatus(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
}


void ControllerTicketStatus::serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    serviceOK(request, response, toList(db()->ticketStatus()));
}


