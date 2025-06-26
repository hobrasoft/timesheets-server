#include "controllerdooremployees.h"
#include "httprequest.h"
#include "db.h"
#include "pdebug.h"
#include "json.h"

using namespace Httpd;

ControllerDoorEmployees::ControllerDoorEmployees(HobrasoftHttpd::HttpConnection *parent)
    : AbstractController(parent) {}

void ControllerDoorEmployees::service(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response)
{
    serviceError(request, response, 405, "bad-request", "Invalid request");
}

