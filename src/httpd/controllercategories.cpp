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


void ControllerCategories::serviceIdGet(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    auto list = db()->categories(id);
    if (list.isEmpty()) {
        serviceError(request, response, 404, "not-found", "NotFound");
        return;
        }
    serviceOK(request, response, list[0].toMap());
}


void ControllerCategories::serviceIdPut(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceOK(request, response, putKey(db()->save(Dbt::Categories::fromMap(data))));
}


void ControllerCategories::serviceIdPost(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceIdPut(request, response, data);
}


void ControllerCategories::serviceIdDelete(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    db()->remove(Dbt::Categories(id));
    serviceOK(request, response);
}

