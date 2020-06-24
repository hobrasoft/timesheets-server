/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controllerstatusorder.h"
#include "db.h"

using namespace Httpd;


ControllerStatusOrder::ControllerStatusOrder(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
}


void ControllerStatusOrder::serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    serviceOK(request, response, toList(DB->statusOrder()));
}


