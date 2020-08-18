/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controlleruserscategories.h"
#include "httprequest.h"
#include "db.h"

using namespace Httpd;


ControllerUsersCategories::ControllerUsersCategories(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
}


void ControllerUsersCategories::serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    int user = (request->parameter("user") == "") ? -1 : request->parameter("user").toInt();
    QString category = (request->parameter("category") == "") ? QString() : request->parameter("user");
    serviceOK(request, response, toList(db()->usersCategories(-1, user, category)));
}


void ControllerUsersCategories::serviceIdGet(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    auto list = db()->usersCategories(id.toInt());
    if (list.isEmpty()) {
        serviceError(request, response, 404, "not-found", "NotFound");
        return;
        }
    serviceOK(request, response, list[0].toMap());
}


void ControllerUsersCategories::serviceIdPut(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceOK(request, response, putKey(db()->save(Dbt::UsersCategories::fromMap(data))));
}


void ControllerUsersCategories::serviceIdPost(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceIdPut(request, response, data);
}


void ControllerUsersCategories::serviceIdDelete(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    db()->remove(Dbt::UsersCategories(id.toInt()));
    serviceOK(request, response);
}

