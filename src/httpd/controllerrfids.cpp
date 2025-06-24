#include "controllerrfids.h"
#include "httprequest.h"
#include "db.h"
#include "pdebug.h"

using namespace Httpd;

ControllerRfids::ControllerRfids(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
    PDEBUG;
}

void ControllerRfids::serviceList(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    PDEBUG;
    serviceOK(request, response, toList(db()->rfids()));
}

void ControllerRfids::serviceIdGet(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    PDEBUG;
    int rfid = id.toInt();
    if (rfid <= 0) {
        serviceError(request, response, 404, "not-found", "NotFound");
        return;
    }
    auto list = db()->rfids(rfid);
    if (list.isEmpty()) {
        serviceError(request, response, 404, "not-found", "NotFound");
        return;
    }
    serviceOK(request, response, list[0].toMap());
}

void ControllerRfids::serviceIdPut(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceOK(request, response, putKey(db()->save(Dbt::Rfids::fromMap(data))));
}

void ControllerRfids::serviceIdPost(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceIdPut(request, response, data);
}

void ControllerRfids::serviceIdDelete(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    db()->remove(Dbt::Rfids(id.toInt()));
    serviceOK(request, response);
}

