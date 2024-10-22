/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controllerdoors.h"
#include "httprequest.h"
#include "db.h"
#include "pdebug.h"

using namespace Httpd;


ControllerDoors::ControllerDoors(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
    PDEBUG;
}


void ControllerDoors::serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    PDEBUG;
    serviceOK(request, response, toList(db()->doors()));
}


void ControllerDoors::serviceIdGet (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    PDEBUG;
    int door = id.toInt();
    if (door <= 0) {
        serviceError(request, response, 404, "not-found", "NotFound");
        return;
        }
    auto list = db()->doors(door);
    if (list.isEmpty()) {
        serviceError(request, response, 404, "not-found", "NotFound");
        return;
        }
    serviceOK(request, response, list[0].toMap());
}


void ControllerDoors::serviceIdPut(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceOK(request, response, putKey(db()->save(Dbt::Doors::fromMap(data))));
}


void ControllerDoors::serviceIdPost(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceIdPut(request, response, data);
}


void ControllerDoors::serviceIdDelete(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    db()->remove(Dbt::Doors(id.toInt()));
    serviceOK(request, response);
}




