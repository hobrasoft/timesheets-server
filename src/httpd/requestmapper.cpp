/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "requestmapper.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "httpconnection.h"
#include "sessionstore.h"
#include "staticfilecontroller.h"
#include "msettings.h"
#include "pdebug.h"
#include "pluginstore.h"
#include "plugininterface.h"
#include "requestauthorizer.h"

// nové API
#include "controllerserver.h"
#include "controllercategories.h"
#include "controllercategoriestoroot.h"
#include "controllercategoriestree.h"
#include "controllerstatusorder.h"
#include "controllerstatuses.h"
#include "controllerstatustemplates.h"
#include "controllertickets.h"
#include "controllerticketsvw.h"
#include "controllerticketstatus.h"
#include "controllerticketvalues.h"
#include "controllerticketfiles.h"
#include "controllertickettimesheets.h"
#include "controllerusers.h"
#include "controlleruserscategories.h"
#include "controllertimesheet.h"
#include "controlleroverview.h"
#include "controlleroverviewpublic.h"

using namespace HobrasoftHttpd;
using namespace Httpd;

/**
 * @addtogroup konfigurace
 * @{
 * Konfigurace Httpd::Httpd
 * ------------
 *
 * - __httpd/user__ - uživatelské jméno (admin)
 * - __httpd/password__ - heslo (admin)
 * - __httpd/authorize__ - pokud je true, přístup na stránky bude vyžadovat heslo (true)
 * @}
 */


RequestMapper::~RequestMapper() {
}


RequestMapper::RequestMapper(HobrasoftHttpd::HttpConnection *parent) : HobrasoftHttpd::HttpRequestHandler(parent) {
    // PDEBUG << parent->thread() << this->thread();
    m_authorizer = new RequestAuthorizer(parent);
}


void RequestMapper::service(HttpRequest *request, HttpResponse *response) {
    m_path = request->path();
    PDEBUG << request->method() << request->fullPath();

    #define ROUTER(adresa, trida) \
        if (m_path.startsWith(adresa)) { \
            AbstractController *controller = new trida (connection()); \
            controller->setAuthorizer(m_authorizer); \
            controller->service(request, response); \
            return; \
            }

    if (m_path.contains(QRegExp("^/public/.*\\.shtml"))) {
        serviceShtmlFile(request, response);
        response->flush();
        return;
        }

    if (m_path.startsWith("/index.html") || m_path == "/") {
        StaticFileController(connection()).service(request, response);
        response->flush();
        return;
        }

    if (m_path.startsWith("/public/")) {
        StaticFileController(connection()).service(request, response);
        response->flush();
        return;
        }


    // Možné bez přihlášení
    ROUTER("/api/v1/overview/0x",       ControllerOverviewPublic);
    ROUTER("/api/v1/server/about",      ControllerServer);

    // Kontrola přihlášení
    if (!m_authorizer->isLoggedIn(request, response)) {
        return;
        }

    /**
     * Po přihlášení zpracovává ostatní speciální požadavky (event streamy).
     */
    ROUTER("/api/v1/tickettimesheets",                 ControllerTicketTimesheets);
    ROUTER("/api/v1/categoriestoroot",                 ControllerCategoriesToRoot);
    ROUTER("/api/v1/statustemplates",                  ControllerStatusTemplates);
    ROUTER("/api/v1/userscategories",                  ControllerUsersCategories);
    ROUTER("/api/v1/categoriestree",                   ControllerCategoriesTree);
    ROUTER("/api/v1/ticketvalues",                     ControllerTicketValues);
    ROUTER("/api/v1/ticketstatus",                     ControllerTicketStatus);
    ROUTER("/api/v1/ticketfiles",                      ControllerTicketFiles);
    ROUTER("/api/v1/statusorder",                      ControllerStatusOrder);
    ROUTER("/api/v1/categories",                       ControllerCategories);
    ROUTER("/api/v1/ticketsvw",                        ControllerTicketsVw);
    ROUTER("/api/v1/timesheet",                        ControllerTimesheet);
    ROUTER("/api/v1/overview",                         ControllerOverview);
    ROUTER("/api/v1/statuses",                         ControllerStatuses);
    ROUTER("/api/v1/tickets",                          ControllerTickets);
    ROUTER("/api/v1/server",                           ControllerServer);
    ROUTER("/api/v1/users",                            ControllerUsers);

    if (m_path.contains(QRegExp(".*\\.shtml"))) {
        serviceShtmlFile(request, response);
        response->flush();
        return;
        }

    StaticFileController(connection()).service(request, response);
}


void RequestMapper::serviceShtmlFile(HttpRequest *request, HttpResponse *response) {
    Q_UNUSED(request);
    QByteArray data;
    try {
        data = readFile(m_path, response);
        }
    catch (...) {
        return;
        }
    response->setHeader("Content-Type", "text/html; charset=" + connection()->settings()->encoding());
    response->write(data);
    response->flush();
}


QByteArray RequestMapper::readFile(const QString& path, HttpResponse *response) {
    QByteArray data;
    if (path.startsWith("/..") || path.startsWith("..")) {
        response->setStatus(403,"Forbidden");
        response->write(QString("403 Forbidden: %1<br>\nDo not use ../ in your file path").arg(path).toUtf8());
        response->flush();
        throw false;
        }

    QFile file(connection()->settings()->docroot()+"/"+path);
    if (!file.exists()) {
        response->setStatus(404, "Not found");
        response->write(QString("404 File not found: %1").arg(file.fileName()).toUtf8());
        response->flush();
        throw false;
        }

    if (!file.open(QIODevice::ReadOnly)) {
        response->setStatus(403, "Forbidden");
        response->write(QString("403 Forbidden: %1").arg(file.fileName()).toUtf8());
        response->flush();
        throw false;
        }

    QString rolename = Security::Roles::toString(m_authorizer->role());

    while (!file.atEnd()) {
        QString line = QString::fromUtf8(file.readLine());
        line = line.replace("${ROLE}", rolename);
        // if (line.contains(QRegExp("^\\s*<!--\\s*#include\\s+\".+\"\\s*-->\\s*$"))) {
        if (line.contains(QRegExp(R"X(^\s*<!--\s*#include\s+['"].+['"]\s*-->\s*$)X"))) {
            QStringList lineparts = line.split(QRegExp(R"X(['"])X"));
            if (lineparts.size() != 3) {
                continue;
                }
            data += readFile(lineparts[1], response);
            continue;
            }
        data += line.toUtf8();
        }

    file.close();
    return data;
}




