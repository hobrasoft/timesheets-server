#include "controllerdooremployees.h"
#include "httprequest.h"
#include "db.h"
#include "pdebug.h"
#include "json.h"
#include <QRegularExpression>

using namespace Httpd;

ControllerDoorEmployees::ControllerDoorEmployees(HobrasoftHttpd::HttpConnection *parent)
    : AbstractController(parent) {}

void ControllerDoorEmployees::service(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response)
{
    static const QRegularExpression re("^/api/v1/doors/(\\d+)/employees(?:/(\\d+))?/?$");
    auto m = re.match(request->path());
    if (!m.hasMatch()) {
        serviceError(request, response, 405, "bad-request", "Invalid request");
        return;
    }

    int door = m.captured(1).toInt();
    int employee = m.captured(2).isEmpty() ? -1 : m.captured(2).toInt();

    if (request->method() == "GET") {
        serviceGet(request, response, door, employee);
        return;
    }

    if (request->method() == "PUT") {
        bool ok = false;
        QVariantMap data = JSON::data(request->body(), &ok).toMap();
        if (!ok) {
            serviceError(request, response, 400, "bad-request", "Could not parse JSON data");
            return;
        }
        servicePut(request, response, door, data);
        return;
    }

    if (request->method() == "DELETE") {
        if (employee <= 0) {
            serviceError(request, response, 400, "bad-request", "Employee id required");
            return;
        }
        serviceDelete(request, response, door, employee);
        return;
    }

    serviceError(request, response, 405, "bad-request", "Invalid request");
}

void ControllerDoorEmployees::serviceGet(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, int door, int employee)
{
    Dbt::EmployeeCanOpenDoor filter;
    filter.door = door;
    filter.employee = employee > 0 ? employee : 0;
    auto list = db()->employeeCanOpenDoor(filter);
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

void ControllerDoorEmployees::servicePut(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, int door, const QVariantMap &data)
{
    QVariantMap d = data;
    d["door"] = door;
    db()->save(Dbt::EmployeeCanOpenDoor::fromMap(d));
    serviceOK(request, response);
}

void ControllerDoorEmployees::serviceDelete(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, int door, int employee)
{
    db()->remove(Dbt::EmployeeCanOpenDoor(employee, door));
    serviceOK(request, response);
}

