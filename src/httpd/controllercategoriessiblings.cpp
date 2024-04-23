/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controllercategoriessiblings.h"
#include "httprequest.h"
#include "pdebug.h"
#include "db.h"

using namespace Httpd;


ControllerCategoriesSiblings::ControllerCategoriesSiblings(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
}


void ControllerCategoriesSiblings::serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    return serviceIdGet(request, response, QString());
}


void ControllerCategoriesSiblings::serviceIdGet(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    serviceOK(request, response, toList(db()->siblingcategories(id)));
}

