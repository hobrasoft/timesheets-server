/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controllercategoriestoroot.h"
#include "db.h"

using namespace Httpd;


ControllerCategoriesToRoot::ControllerCategoriesToRoot(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
}


void ControllerCategoriesToRoot::serviceIdGet(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    auto list = db()->categoriesToRoot(id);
    if (list.isEmpty()) {
        serviceError(request, response, 404, "not-found", "NotFound");
        return;
        }
    serviceOK(request, response, toList(list));
}

