/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controlleroverviewpublic.h"
#include "httprequest.h"
#include "db.h"
#include "msettings.h"
#include "pdebug.h"

using namespace Httpd;


ControllerOverviewPublic::ControllerOverviewPublic(HobrasoftHttpd::HttpConnection *parent) : AbstractController (parent) {
}


void ControllerOverviewPublic::serviceIdGet (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    QString remappedId = id;
    remappedId = remappedId.replace("0x","");
    auto overview = db()->overview(remappedId);
    serviceOK(request, response, overview.first().toMap());
}




