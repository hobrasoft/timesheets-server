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
    serviceOK(request, response, toList(db()->categories(id)));
}


void ControllerCategories::serviceIdPut(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    db()->save(Dbt::Categories::fromMap(data));
}


void ControllerCategories::serviceIdPost(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceIdPut(request, response, data);
}


void ControllerCategories::serviceIdDelete(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    db()->remove(Dbt::Categories(id));
    serviceOK(request, response);
}

