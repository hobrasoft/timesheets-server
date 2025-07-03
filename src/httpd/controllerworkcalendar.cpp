#include "controllerworkcalendar.h"
#include "httprequest.h"
#include "db.h"
#include <QDate>

using namespace Httpd;

ControllerWorkCalendar::ControllerWorkCalendar(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) { }

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
