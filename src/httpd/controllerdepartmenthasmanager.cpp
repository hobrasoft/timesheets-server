#include "controllerdepartmenthasmanager.h"
#include "httprequest.h"
#include "db.h"
#include "pdebug.h"
#include "json.h"
#include <QRegularExpression>

using namespace Httpd;

ControllerDepartmentHasManager::ControllerDepartmentHasManager(HobrasoftHttpd::HttpConnection *parent)
    : AbstractController(parent) {}

void ControllerDepartmentHasManager::service(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response)
{
    static const QRegularExpression re("^/api/v1/departments/(\\d+)/managers(?:/(\\d+))?/?$");
    auto m = re.match(request->path());
    if (!m.hasMatch()) {
        serviceError(request, response, 405, "bad-request", "Invalid request");
        return;
    }

    int department = m.captured(1).toInt();
    int user = m.captured(2).isEmpty() ? -1 : m.captured(2).toInt();

    if (request->method() == "GET") {
        serviceGet(request, response, department, user);
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
        if (user <= 0) {
            serviceError(request, response, 400, "bad-request", "User id required");
            return;
        }
        serviceDelete(request, response, department, user);
        return;
    }

    serviceError(request, response, 405, "bad-request", "Invalid request");
}

void ControllerDepartmentHasManager::serviceGet(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, int department, int user)
{
    Dbt::DepartmentHasManager filter;
    filter.department = department;
    filter.user = user > 0 ? user : 0;
    auto list = db()->departmentHasManager(filter);
    if (user > 0) {
        if (list.isEmpty()) {
            serviceError(request, response, 404, "not-found", "NotFound");
            return;
        }
        serviceOK(request, response, list.first().toMap());
    } else {
        serviceOK(request, response, toList(list));
    }
}

void ControllerDepartmentHasManager::servicePut(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, int department, const QVariantMap &data)
{
    QVariantMap d = data;
    d["department"] = department;
    db()->save(Dbt::DepartmentHasManager::fromMap(d));
    serviceOK(request, response);
}

void ControllerDepartmentHasManager::serviceDelete(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, int department, int user)
{
    db()->remove(Dbt::DepartmentHasManager(department, user));
    serviceOK(request, response);
}

