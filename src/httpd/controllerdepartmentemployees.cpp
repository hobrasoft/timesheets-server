#include "controllerdepartmentemployees.h"
#include "httprequest.h"
#include "db.h"
#include "pdebug.h"
#include "json.h"

using namespace Httpd;

ControllerDepartmentEmployees::ControllerDepartmentEmployees(HobrasoftHttpd::HttpConnection *parent)
    : AbstractController(parent) {}

void ControllerDepartmentEmployees::service(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response)
{
    serviceError(request, response, 405, "bad-request", "Invalid request");
}

