/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controllerevents.h"
#include "httprequest.h"
#include "db.h"
#include "pdebug.h"
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
    serviceOK(request, response, toList(db()->events(-1, employee, month, limit, offset)));
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
    serviceOK(request, response, list[0].toMap());
}

void ControllerEvents::serviceIdPut(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceOK(request, response, putKey(db()->save(Dbt::Events::fromMap(data))));
}

void ControllerEvents::serviceIdPost(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceIdPut(request, response, data);
}

void ControllerEvents::serviceIdDelete(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    db()->remove(Dbt::Events(id.toInt()));
    serviceOK(request, response);
}

