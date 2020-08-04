/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controllerusers.h"
#include "db.h"

using namespace Httpd;


ControllerUsers::ControllerUsers(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
}


void ControllerUsers::serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    serviceOK(request, response, toList(db()->users()));
}


void ControllerUsers::serviceIdPut(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    db()->save(Dbt::Users::fromMap(data));
}


void ControllerUsers::serviceIdPost(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceIdPut(request, response, data);
}


void ControllerUsers::serviceIdDelete(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    db()->remove(Dbt::Users(id.toInt()));
    serviceOK(request, response);
}

