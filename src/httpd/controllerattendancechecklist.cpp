#include "controllerattendancechecklist.h"
#include "httprequest.h"
#include "db.h"
#include <QDate>

using namespace Httpd;

ControllerAttendanceChecklist::ControllerAttendanceChecklist(HobrasoftHttpd::HttpConnection *parent)
    : AbstractController(parent) {}

void ControllerAttendanceChecklist::serviceList(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response)
{
    int employee = request->parameter("employee").toInt();
    QString smonth = request->parameter("month");
    QDate month = QDate::fromString(smonth, "yyyy-MM-dd");
    serviceOK(request, response, toList(db()->attendanceChecklist(employee, month)));
}

