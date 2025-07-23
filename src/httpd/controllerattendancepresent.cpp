#include "controllerattendancepresent.h"
#include "httprequest.h"
#include "db.h"

using namespace Httpd;

ControllerAttendancePresent::ControllerAttendancePresent(HobrasoftHttpd::HttpConnection *parent)
    : AbstractController(parent) {}

void ControllerAttendancePresent::serviceList(HobrasoftHttpd::HttpRequest *request,
                                              HobrasoftHttpd::HttpResponse *response) {
    Q_UNUSED(request);
    serviceOK(request, response, toList(db()->attendancePresent()));
}
