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
    QStringList parts = request->path().split("/");
    // remove /, api, v1, departments
    if (parts.size() < 6) {
        serviceError(request, response, 404, "not-found", "Not found");
        return;
    }
    parts.removeFirst();
    parts.removeFirst();
    parts.removeFirst();
    parts.removeFirst();
    bool ok = false;
    int department = parts.takeFirst().toInt(&ok);
    if (!ok || parts.takeFirst() != "employees") {
        serviceError(request, response, 404, "not-found", "Not found");
        return;
    }

    if (request->method() == "GET" && parts.isEmpty()) {
        Dbt::DepartmentHasMember p; p.department = department; p.employee = 0;
        serviceOK(request, response, toList(db()->departmentHasMember(p)));
        return;
    }

    if ((request->method() == "PUT" || request->method() == "POST") && parts.isEmpty()) {
        bool okJson = false;
        QVariantMap data = JSON::data(request->body(), &okJson).toMap();
        if (!okJson) {
            serviceError(request, response, 400, "bad-request", "Invalid JSON");
            return;
        }
        Dbt::DepartmentHasMember item = Dbt::DepartmentHasMember::fromMap(data);
        if (item.department == 0)
            item.department = department;
        serviceOK(request, response, putKey(db()->save(item)));
        return;
    }

    if (request->method() == "DELETE" && parts.size() == 1) {
        int employee = parts[0].toInt(&ok);
        if (!ok) {
            serviceError(request, response, 400, "bad-request", "Invalid employee");
            return;
        }
        Dbt::DepartmentHasMember item; item.department = department; item.employee = employee;
        db()->remove(item);
        serviceOK(request, response);
        return;
    }

    serviceError(request, response, 405, "bad-request", "Invalid request");
}

