/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controllertickets.h"
#include "httprequest.h"
#include "db.h"
#include "pdebug.h"

using namespace Httpd;


ControllerTickets::ControllerTickets(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
}


void ControllerTickets::serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    bool all = QVariant(request->parameter("all")).toBool();
    bool idonly = QVariant(request->parameter("idonly")).toBool();
    if (idonly) {
        QVariantList data;
        QList<Dbt::Tickets> list = db()->tickets(all);
        QListIterator<Dbt::Tickets> iterator(list);
        while (iterator.hasNext()) {
            data << iterator.next().ticket;
            }
        serviceOK(request, response, data);
        return;
        }
    serviceOK(request, response, toList(db()->tickets(all)));
}


void ControllerTickets::serviceIdGet (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    bool all = QVariant(request->parameter("all")).toBool();
    auto list = db()->tickets(id.toInt(), all);
    if (list.isEmpty()) {
        serviceError(request, response, 404, "not-found", "NotFound");
        return;
        }
    serviceOK(request, response, list[0].toMap());
}


void ControllerTickets::serviceIdPut(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceOK(request, response, putKey(db()->save(Dbt::Tickets::fromMap(data))));
}


void ControllerTickets::serviceIdPost(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceIdPut(request, response, data);
}


void ControllerTickets::serviceIdDelete(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    db()->remove(Dbt::Tickets(id));
    serviceOK(request, response);
}




