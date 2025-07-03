#include "controllerworkcalendar.h"
#include "httprequest.h"
#include "db.h"
#include <QDate>

using namespace Httpd;

ControllerWorkCalendar::ControllerWorkCalendar(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) { }

void ControllerWorkCalendar::service(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    QStringList parts = request->path().split("/");
    parts.removeFirst();
    parts.removeFirst();
    parts.removeFirst();

    if (parts.size() == 3 && parts[0] == "workcalendar" && parts[1] == "generate") {
        bool ok = false;
        int year = parts[2].toInt(&ok);
        if (!ok) {
            serviceError(request, response, 400, "bad-request", "Invalid year");
            return;
        }
        serviceGenerate(request, response, year);
        return;
    }

    AbstractController::service(request, response);
}

void ControllerWorkCalendar::serviceList(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    Q_UNUSED(request);
    serviceOK(request, response, toList(db()->workCalendar()));
}

void ControllerWorkCalendar::serviceIdGet(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    QDate date = QDate::fromString(id, "yyyy-MM-dd");
    if (!date.isValid()) { serviceError(request, response, 404, "not-found", "NotFound"); return; }
    auto list = db()->workCalendar(date);
    if (list.isEmpty()) { serviceError(request, response, 404, "not-found", "NotFound"); return; }
    serviceOK(request, response, list.first().toMap());
}

void ControllerWorkCalendar::serviceIdPut(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    Q_UNUSED(request);
    serviceOK(request, response, putKey(db()->save(Dbt::WorkCalendar::fromMap(data))));
}

void ControllerWorkCalendar::serviceIdPost(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceIdPut(request, response, data);
}

void ControllerWorkCalendar::serviceIdDelete(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    QDate date = QDate::fromString(id, "yyyy-MM-dd");
    db()->remove(Dbt::WorkCalendar(date));
    serviceOK(request, response);
}

void ControllerWorkCalendar::serviceGenerate(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, int year) {
    Q_UNUSED(request);
    db()->generateWorkCalendar(year);
    serviceOK(request, response);
}
