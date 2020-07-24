/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controllercategories.h"
#include "db.h"

using namespace Httpd;


ControllerCategories::ControllerCategories(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
}


void ControllerCategories::serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    serviceOK(request, response, toList(db()->categories()));
}


