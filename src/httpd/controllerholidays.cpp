/**
 * @file
 */

#include "controllerholidays.h"
#include "httprequest.h"
#include "db.h"
#include "pdebug.h"
#include <QDate>

using namespace Httpd;

ControllerHolidays::ControllerHolidays(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) { }

void ControllerHolidays::service(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    QStringList parts = request->path().split("/");
    parts.removeFirst();
    parts.removeFirst();
    parts.removeFirst();

    if (parts.size() == 4 && parts[0] == "holidays" && parts[1] == "copy") {
        bool ok1 = false; bool ok2 = false;
        int fromYear = parts[2].toInt(&ok1);
        int toYear = parts[3].toInt(&ok2);
        if (!ok1 || !ok2) {
            serviceError(request, response, 400, "bad-request", "Invalid year");
            return;
        }
        serviceCopy(request, response, fromYear, toYear);
        return;
    }

    AbstractController::service(request, response);
}

void ControllerHolidays::serviceList(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    Q_UNUSED(request);
    serviceOK(request, response, toList(db()->holidays()));
}

void ControllerHolidays::serviceIdGet(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    QDate date = QDate::fromString(id, "yyyy-MM-dd");
    if (!date.isValid()) { serviceError(request, response, 404, "not-found", "NotFound"); return; }
    auto list = db()->holidays(date);
    if (list.isEmpty()) { serviceError(request, response, 404, "not-found", "NotFound"); return; }
    serviceOK(request, response, list.first().toMap());
}

void ControllerHolidays::serviceIdPut(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    Q_UNUSED(request);
    serviceOK(request, response, putKey(db()->save(Dbt::Holidays::fromMap(data))));
}

void ControllerHolidays::serviceIdPost(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceIdPut(request, response, data);
}

void ControllerHolidays::serviceIdDelete(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    QDate date = QDate::fromString(id, "yyyy-MM-dd");
    db()->remove(Dbt::Holidays(date));
    serviceOK(request, response);
}

void ControllerHolidays::serviceCopy(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, int fromYear, int toYear) {
    Q_UNUSED(request);
    db()->copyHolidays(fromYear, toYear);
    serviceOK(request, response);
}

