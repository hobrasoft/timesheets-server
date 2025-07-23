#include "controllerattendancechecklist.h"
#include "httprequest.h"
#include "db.h"
#include <QDate>

using namespace Httpd;

ControllerAttendanceChecklist::ControllerAttendanceChecklist(HobrasoftHttpd::HttpConnection *parent)
    : AbstractController(parent) {}


void ControllerAttendanceChecklist::serviceList(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    int employee = request->parameter("employee").toInt();
    QString smonth = request->parameter("month");
    QDate month = QDate::fromString(smonth, "yyyy-MM-dd");

    if (employee == 0) {
        serviceOK(request, response, toList(db()->attendanceChecklist(month)));
        return;
        }

    if (!db()->canAccessAttendance(employee)) {
        serviceError(request, response, 403, tr("Permission denied"), tr("Cannot access this employee"));
        return;
        }
    serviceOK(request, response, toList(db()->attendanceChecklist(employee, month)));
}

