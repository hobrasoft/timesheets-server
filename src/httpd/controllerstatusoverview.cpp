#include "controllerstatusoverview.h"
#include "httprequest.h"
#include "db.h"

using namespace Httpd;

ControllerStatusOverview::ControllerStatusOverview(HobrasoftHttpd::HttpConnection *parent)
    : AbstractController(parent) {}

void ControllerStatusOverview::serviceIdGet(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    Q_UNUSED(request);
    auto list = db()->statusOverview(id);
    serviceOK(request, response, toList(list));
}
