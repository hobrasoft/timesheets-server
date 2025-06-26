#include "controllerrfidemployees.h"
#include "httprequest.h"
#include "db.h"
#include "pdebug.h"
#include "json.h"

using namespace Httpd;

ControllerRfidEmployees::ControllerRfidEmployees(HobrasoftHttpd::HttpConnection *parent)
    : AbstractController(parent) {}

void ControllerRfidEmployees::service(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response)
{
    serviceError(request, response, 405, "bad-request", "Invalid request");
}

