/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controllerevents.h"
#include "httprequest.h"
#include "db.h"
#include "pdebug.h"
#include <QSet>
#include <QDate>

using namespace Httpd;

ControllerEvents::ControllerEvents(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
    PDEBUG;
}

void ControllerEvents::serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    PDEBUG;
    int offset   = request->parameter("offset").toInt();
    int limit    = request->parameter("limit").toInt();
    int employee = request->parameter("employee").toInt();
    QString smonth = request->parameter("month");
    QDate month;
    if (!smonth.isEmpty()) {
        month = QDate::fromString(smonth + "-01", "yyyy-MM-dd");
    }
    QList<Dbt::Events> list = db()->events(-1, employee, month, limit, offset);
    list = filterAccessible(list);
    serviceOK(request, response, toList(list));
}

void ControllerEvents::serviceIdGet (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    PDEBUG;
    int event = id.toInt();
    if (event <= 0) {
        serviceError(request, response, 404, "not-found", "NotFound");
        return;
        }
    auto list = db()->events(event);
    if (list.isEmpty()) {
        serviceError(request, response, 404, "not-found", "NotFound");
        return;
        }
    if (!canAccessEmployee(list.first().employee)) {
        serviceError(request, response, 403, "forbidden", "Forbidden");
        return;
        }
    serviceOK(request, response, list[0].toMap());
}

void ControllerEvents::serviceIdPut(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    QVariantMap d = data;
    d["user_edited"] = authenticatedUser()->user();
    if (!canAccessEmployee(d["employee"].toInt())) {
        serviceError(request, response, 403, "forbidden", "Forbidden");
        return;
        }
    serviceOK(request, response, putKey(db()->save(Dbt::Events::fromMap(d))));
}

void ControllerEvents::serviceIdPost(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceIdPut(request, response, data);
}

void ControllerEvents::serviceIdDelete(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    int evId = id.toInt();
    auto list = db()->events(evId);
    if (list.isEmpty()) {
        serviceError(request, response, 404, "not-found", "NotFound");
        return;
        }
    Dbt::Events e = list.first();
    if (!canAccessEmployee(e.employee)) {
        serviceError(request, response, 403, "forbidden", "Forbidden");
        return;
        }
    e.valid = false;
    e.user_edited = authenticatedUser()->user();
    db()->save(e);
    serviceOK(request, response);
}

bool ControllerEvents::canAccessEmployee(int employee) {
    if (authenticatedUser() == nullptr || authenticatedUser()->admin()) {
        return true;
        }
    QList<Dbt::DepartmentHasManager> mng = db()->departmentHasManager(Dbt::DepartmentHasManager(0, authenticatedUser()->user()));
    for (const auto& mm : mng) {
        if (!db()->departmentHasMember(Dbt::DepartmentHasMember(mm.department, employee)).isEmpty()) {
            return true;
            }
        }
    return false;
}

QList<Dbt::Events> ControllerEvents::filterAccessible(const QList<Dbt::Events>& list) {
    if (authenticatedUser() == nullptr || authenticatedUser()->admin()) {
        return list;
        }
    QSet<int> allowed;
    QList<Dbt::DepartmentHasManager> mng = db()->departmentHasManager(Dbt::DepartmentHasManager(0, authenticatedUser()->user()));
    for (const auto& mm : mng) {
        auto members = db()->departmentHasMember(Dbt::DepartmentHasMember(mm.department, 0));
        for (const auto& mem : members) { allowed.insert(mem.employee); }
        }
    QList<Dbt::Events> out;
    for (const auto& e : list) {
        if (allowed.contains(e.employee)) out << e;
        }
    return out;
}

