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
    QStringList parts = request->path().split("/");
    if (parts.size() < 6) {
        serviceError(request, response, 404, "not-found", "Not found");
        return;
    }
    parts.removeFirst();
    parts.removeFirst();
    parts.removeFirst();
    parts.removeFirst();
    bool ok = false;
    int rfid = parts.takeFirst().toInt(&ok);
    if (!ok || parts.takeFirst() != "employees") {
        serviceError(request, response, 404, "not-found", "Not found");
        return;
    }

    if (request->method() == "GET" && parts.isEmpty()) {
        Dbt::EmployeeHasRfid p; p.rfid = rfid; p.employee = 0;
        serviceOK(request, response, toList(db()->employeeHasRfid(p)));
        return;
    }

    if ((request->method() == "PUT" || request->method() == "POST") && parts.isEmpty()) {
        bool okJson = false;
        QVariantMap data = JSON::data(request->body(), &okJson).toMap();
        if (!okJson) {
            serviceError(request, response, 400, "bad-request", "Invalid JSON");
            return;
        }
        Dbt::EmployeeHasRfid item = Dbt::EmployeeHasRfid::fromMap(data);
        if (item.rfid == 0)
            item.rfid = rfid;
        serviceOK(request, response, putKey(db()->save(item)));
        return;
    }

    if (request->method() == "DELETE" && parts.size() == 1) {
        int employee = parts[0].toInt(&ok);
        if (!ok) {
            serviceError(request, response, 400, "bad-request", "Invalid employee");
            return;
        }
        Dbt::EmployeeHasRfid item; item.rfid = rfid; item.employee = employee;
        db()->remove(item);
        serviceOK(request, response);
        return;
    }

    serviceError(request, response, 405, "bad-request", "Invalid request");
}

