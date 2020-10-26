/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controllertimesheet.h"
#include "httprequest.h"
#include "db.h"
#include "pdebug.h"

using namespace Httpd;


ControllerTimesheet::ControllerTimesheet(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
}


void ControllerTimesheet::serviceStart(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, int id) {
    PDEBUG;
    QList<Dbt::TicketTimesheets> list1 = db()->runningTimesheets(id);
    if (!list1.isEmpty()) {
        QVariantMap item = list1[0].toMap();
        item.remove("created");
        item.remove("modified");
        item["ok"] = false;
        item["error"] = "conflict";
        item["reason"] = "Timesheet is already started";
        serviceError(request, response, 409, "conflict", item);
        return;
        }

    QList<Dbt::TicketTimesheets> list2 = db()->startTimesheet(id);
    if (list2.isEmpty()) {
        serviceError(request, response, 400, "error", "Timesheet could not be started");
        return;
        }

    QVariantMap item = list2[0].toMap();
    item.remove("created");
    item.remove("modified");
    serviceOK(request, response, item);
}


void ControllerTimesheet::serviceStop(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, int id) {
    PDEBUG;
    QList<Dbt::TicketTimesheets> list1 = db()->runningTimesheets(id);
    if (list1.isEmpty()) {
        PDEBUG << "konflict";
        QVariantMap item;
        item["ok"] = false;
        item["error"] = "conflict";
        item["reason"] = "Timesheet is already stopped";
        serviceError(request, response, 409, "conflict", item);
        return;
        }

    QList<Dbt::TicketTimesheets> list2 = db()->stopTimesheet(id);
    if (list2.isEmpty()) {
        PDEBUG << "error";
        serviceError(request, response, 400, "error", "Timesheet could not be stopped");
        return;
        }

    QVariantMap item = list2[0].toMap();
    item.remove("created");
    item.remove("modified");
    serviceOK(request, response, item);
}


void ControllerTimesheet::serviceToggle(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, int id) {
    PDEBUG;
    QList<Dbt::TicketTimesheets> list2 = db()->toggleTimesheet(id);
    if (list2.isEmpty()) {
        serviceError(request, response, 400, "error", "Timesheet could not be toggled");
        return;
        }

    QVariantMap item = list2[0].toMap();
    item.remove("created");
    item.remove("modified");
    serviceOK(request, response, item);
}


void ControllerTimesheet::service(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    QStringList parts = request->path().split("/");
    PDEBUG << parts;
    parts.removeFirst();
    parts.removeFirst();
    parts.removeFirst();

    while (parts.size() == 3) {
        bool ok;
        int id = parts[2].toInt(&ok);
        if (!ok) { break; }

        if (parts[0] == "timesheet" && parts[1] == "start") {
            serviceStart(request, response, id);
            return;
            }

        if (parts[0] == "timesheet" && parts[1] == "stop") {
            serviceStop(request, response, id);
            return;
            }

        if (parts[0] == "timesheet" && parts[1] == "toggle") {
            serviceToggle(request, response, id);
            return;
            }

        break;
        }

    serviceError(request, response, 405, "bad-request", "Invalid request");
}

