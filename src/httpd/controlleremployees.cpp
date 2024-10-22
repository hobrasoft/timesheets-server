/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controlleremployees.h"
#include "httprequest.h"
#include "db.h"
#include "pdebug.h"

using namespace Httpd;


ControllerEmployees::ControllerEmployees(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
    PDEBUG;
}


void ControllerEmployees::serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    PDEBUG;
    serviceOK(request, response, toList(db()->employees()));
}


void ControllerEmployees::serviceIdGet (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    PDEBUG;
    int employee = id.toInt();
    if (employee <= 0) {
        serviceError(request, response, 404, "not-found", "NotFound");
        return;
        }
    auto list = db()->employees(employee);
    if (list.isEmpty()) {
        serviceError(request, response, 404, "not-found", "NotFound");
        return;
        }
    serviceOK(request, response, list[0].toMap());
}


void ControllerEmployees::serviceIdPut(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceOK(request, response, putKey(db()->save(Dbt::Employees::fromMap(data))));
}


void ControllerEmployees::serviceIdPost(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) {
    serviceIdPut(request, response, data);
}


void ControllerEmployees::serviceIdDelete(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    db()->remove(Dbt::Employees(id.toInt()));
    serviceOK(request, response);
}




