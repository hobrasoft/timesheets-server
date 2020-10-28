/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controllerticketsvw.h"
#include "httprequest.h"
#include "db.h"
#include "pdebug.h"

using namespace Httpd;


ControllerTicketsVw::ControllerTicketsVw(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
}


void ControllerTicketsVw::serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    bool all = QVariant(request->parameter("all")).toBool();
    QString category = request->parameter("category");
    QList<Dbt::TicketsVw> list;
    if (category.isEmpty()) {
        list = db()->ticketsVw(all);
      } else {
        list = db()->ticketsVw(Dbt::Categories(category), all);
        }
    serviceOK(request, response, toList(list));
}


void ControllerTicketsVw::serviceIdGet (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    bool all = QVariant(request->parameter("all")).toBool();
    auto list = db()->ticketsVw(id.toInt(), all);
    if (list.isEmpty()) {
        serviceError(request, response, 404, "not-found", "Not found");
        return;
        }

    serviceOK(request, response, list[0].toMap());
}


void ControllerTicketsVw::serviceIdPut(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceOK(request, response, putKey(db()->save(Dbt::TicketsVw::fromMap(data))));
}


void ControllerTicketsVw::serviceIdPost(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceIdPut(request, response, data);
}


void ControllerTicketsVw::serviceIdDelete(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    db()->remove(Dbt::TicketsVw(id));
    serviceOK(request, response);
}



