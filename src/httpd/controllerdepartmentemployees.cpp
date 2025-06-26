#include "controllerdepartmentemployees.h"
#include "httprequest.h"
#include "db.h"
#include <QRegularExpression>
#include "pdebug.h"
#include "json.h"

using namespace Httpd;

ControllerDepartmentEmployees::ControllerDepartmentEmployees(HobrasoftHttpd::HttpConnection *parent)
    : AbstractController(parent) {}

void ControllerDepartmentEmployees::service(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response)
{
    static const QRegularExpression re("^/api/v1/departments/(\\d+)/employees(?:/(\\d+))?/?$");
    auto m = re.match(request->path());
    if (!m.hasMatch()) {
        serviceError(request, response, 405, "bad-request", "Invalid request");
        return;
    }

    int department = m.captured(1).toInt();
    int employee = m.captured(2).isEmpty() ? -1 : m.captured(2).toInt();

    if (request->method() == "GET") {
        serviceGet(request, response, department, employee);
        return;
    }

    if (request->method() == "PUT") {
        bool ok = false;
        QVariantMap data = JSON::data(request->body(), &ok).toMap();
        if (!ok) {
            serviceError(request, response, 400, "bad-request", "Could not parse JSON data");
            return;
        }
        servicePut(request, response, department, data);
        return;
    }

    if (request->method() == "DELETE") {
        if (employee <= 0) {
            serviceError(request, response, 400, "bad-request", "Employee id required");
            return;
        }
        serviceDelete(request, response, department, employee);
        return;
    }

    serviceError(request, response, 405, "bad-request", "Invalid request");
}

void ControllerDepartmentEmployees::serviceGet(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, int department, int employee)
{
    Dbt::DepartmentHasMember filter;
    filter.department = department;
    filter.employee = employee > 0 ? employee : 0;
    auto list = db()->departmentHasMember(filter);
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

void ControllerDepartmentEmployees::servicePut(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, int department, const QVariantMap &data)
{
    QVariantMap d = data;
    d["department"] = department;
    db()->save(Dbt::DepartmentHasMember::fromMap(d));
    serviceOK(request, response);
}

void ControllerDepartmentEmployees::serviceDelete(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, int department, int employee)
{
    db()->remove(Dbt::DepartmentHasMember(department, employee));
    serviceOK(request, response);
}

