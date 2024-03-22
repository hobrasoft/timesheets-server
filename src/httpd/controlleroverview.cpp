/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controlleroverview.h"
#include "httprequest.h"
#include "db.h"
#include "pdebug.h"

using namespace Httpd;


ControllerOverview::ControllerOverview(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
}


void ControllerOverview::serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    serviceOK(request, response, toList(db()->overviewList()));
}


void ControllerOverview::serviceIdGet (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    QStringList statuses = request->parameter("statuses").split(",");
    auto list = db()->categories(id);
    if (list.isEmpty()) {
        serviceError(request, response, 404, "not-found", "Not found");
        return;
        }

    auto overview = db()->overview(id, statuses);

    if (overview.isEmpty()) {
        serviceError(request, response, 404, "not-found", "Not found");
        return;
        }

    serviceOK(request, response, overview.first().toMap());
}


void ControllerOverview::serviceIdDelete (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    db()->remove(Dbt::OverviewList(id));
    serviceOK(request, response);
}



