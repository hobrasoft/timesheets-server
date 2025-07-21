#include "controllerattendancesummary.h"
#include "httprequest.h"
#include "db.h"
#include <QDate>

using namespace Httpd;

ControllerAttendanceSummary::ControllerAttendanceSummary(HobrasoftHttpd::HttpConnection *parent)
    : AbstractController(parent) {}

void ControllerAttendanceSummary::serviceList(HobrasoftHttpd::HttpRequest *request,
                                              HobrasoftHttpd::HttpResponse *response)
{
    int employee = request->parameter("employee").toInt();
    QString smonth = request->parameter("month");
    QDate month = QDate::fromString(smonth, "yyyy-MM-dd");
    serviceOK(request, response, toList(db()->attendanceSummary(employee, month)));
}

void ControllerAttendanceSummary::serviceIdGet(HobrasoftHttpd::HttpRequest *request,
                                               HobrasoftHttpd::HttpResponse *response,
                                               const QString &id)
{
    QDate month = QDate::fromString(id, "yyyy-MM-dd");
    int employee = request->parameter("employee").toInt();
    auto list = db()->attendanceSummary(employee, month);
    if (list.isEmpty()) {
        serviceError(request, response, 404, "not-found", "NotFound");
        return;
    }
    serviceOK(request, response, list.first().toMap());
}

void ControllerAttendanceSummary::serviceIdPut(HobrasoftHttpd::HttpRequest *request,
                                               HobrasoftHttpd::HttpResponse *response,
                                               const QVariantMap &data)
{
    serviceOK(request, response, putKey(db()->save(Dbt::AttendanceSummary::fromMap(data))));
}

void ControllerAttendanceSummary::serviceIdPost(HobrasoftHttpd::HttpRequest *request,
                                                HobrasoftHttpd::HttpResponse *response,
                                                const QVariantMap &data)
{
    serviceIdPut(request, response, data);
}

void ControllerAttendanceSummary::serviceIdDelete(HobrasoftHttpd::HttpRequest *request,
                                                  HobrasoftHttpd::HttpResponse *response,
                                                  const QString &id)
{
    QDate month = QDate::fromString(id, "yyyy-MM-dd");
    int employee = request->parameter("employee").toInt();
    Dbt::AttendanceSummary x;
    x.month = month;
    x.employee = employee;
    db()->remove(x);
    serviceOK(request, response);
}

