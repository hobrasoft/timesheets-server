/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controllerticketfiles.h"
#include "httprequest.h"
#include "db.h"

using namespace Httpd;


ControllerTicketFiles::ControllerTicketFiles(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
}


void ControllerTicketFiles::serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    bool all = QVariant(request->parameter("all")).toBool();
    serviceOK(request, response, toList(db()->ticketFiles(all)));
}


void ControllerTicketFiles::serviceIdGet (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    bool all = QVariant(request->parameter("all")).toBool();
    int ticket = QVariant(request->parameter("ticket")).toInt();
    auto list = (ticket > 0)
                    ? db()->ticketFiles(ticket, all)
                    : (id.isEmpty())
                        ? db()->ticketFiles(all)
                        : db()->ticketFiles(id.toInt())
                        ;
    if (list.isEmpty()) {
        serviceError(request, response, 404, "not-found", "NotFound");
        return;
        }
    serviceOK(request, response, list[0].toMap());
}


void ControllerTicketFiles::serviceIdPut(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceOK(request, response, putKey(db()->save(Dbt::TicketFiles::fromMap(data))));
}


void ControllerTicketFiles::serviceIdPost(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceIdPut(request, response, data);
}


void ControllerTicketFiles::serviceIdDelete(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    db()->remove(Dbt::TicketFiles(id));
    serviceOK(request, response);
}

