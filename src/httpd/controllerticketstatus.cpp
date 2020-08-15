/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controllerticketstatus.h"
#include "httprequest.h"
#include "db.h"

using namespace Httpd;


ControllerTicketStatus::ControllerTicketStatus(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
}


void ControllerTicketStatus::serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    bool all = QVariant(request->parameter("all")).toBool();
    serviceOK(request, response, toList(db()->ticketStatus(all)));
}


void ControllerTicketStatus::serviceIdGet (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    bool all = QVariant(request->parameter("all")).toBool();
    int ticket = QVariant(request->parameter("ticket")).toInt();
    auto list = (ticket > 0)
                    ? db()->ticketStatus(ticket, all)
                    : (id.isEmpty())
                        ? db()->ticketStatus(all)
                        : db()->ticketStatus(id.toInt())
                        ;
    if (list.isEmpty()) {
        serviceError(request, response, 404, "not-found", "NotFound");
        return;
        }
    serviceOK(request, response, list[0].toMap());
}


void ControllerTicketStatus::serviceIdPut(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceOK(request, response, putKey(db()->save(Dbt::TicketStatus::fromMap(data))));
}


void ControllerTicketStatus::serviceIdPost(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceIdPut(request, response, data);
}


void ControllerTicketStatus::serviceIdDelete(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    db()->remove(Dbt::TicketStatus(id));
    serviceOK(request, response);
}

