#include "controllerrfidemployees.h"
#include "httprequest.h"
#include "db.h"
#include "pdebug.h"
#include "json.h"
#include <QRegularExpression>

using namespace Httpd;

ControllerRfidEmployees::ControllerRfidEmployees(HobrasoftHttpd::HttpConnection *parent)
    : AbstractController(parent) {}

void ControllerRfidEmployees::service(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response)
{
    static const QRegularExpression re("^/api/v1/rfids/(\\d+)/employees(?:/(\\d+))?/?$");
    auto m = re.match(request->path());
    if (!m.hasMatch()) {
        serviceError(request, response, 405, "bad-request", "Invalid request");
        return;
    }

    int rfid = m.captured(1).toInt();
    int employee = m.captured(2).isEmpty() ? -1 : m.captured(2).toInt();

    if (request->method() == "GET") {
        serviceGet(request, response, rfid, employee);
        return;
    }

    if (request->method() == "PUT") {
        bool ok = false;
        QVariantMap data = JSON::data(request->body(), &ok).toMap();
        if (!ok) {
            serviceError(request, response, 400, "bad-request", "Could not parse JSON data");
            return;
        }
        servicePut(request, response, rfid, data);
        return;
    }

    if (request->method() == "DELETE") {
        if (employee <= 0) {
            serviceError(request, response, 400, "bad-request", "Employee id required");
            return;
        }
        serviceDelete(request, response, rfid, employee);
        return;
    }

    serviceError(request, response, 405, "bad-request", "Invalid request");
}

void ControllerRfidEmployees::serviceGet(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, int rfid, int employee)
{
    Dbt::EmployeeHasRfid filter;
    filter.rfid = rfid;
    filter.employee = employee > 0 ? employee : 0;
    auto list = db()->employeeHasRfid(filter);
    if (employee > 0) {
        if (list.isEmpty()) {
            serviceError(request, response, 404, "not-found", "NotFound");
            return;
        }
        serviceOK(request, response, list.first().toMap());
    } else {
        serviceOK(request, response, toList(list));
    }
}

void ControllerRfidEmployees::servicePut(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, int rfid, const QVariantMap &data)
{
    QVariantMap d = data;
    d["rfid"] = rfid;
    db()->save(Dbt::EmployeeHasRfid::fromMap(d));
    serviceOK(request, response);
}

void ControllerRfidEmployees::serviceDelete(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, int rfid, int employee)
{
    db()->remove(Dbt::EmployeeHasRfid(employee, rfid));
    serviceOK(request, response);
}

