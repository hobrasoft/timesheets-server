/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "abstractcontroller.h"
#include "httprequesthandler.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "httpconnection.h"
#include "requestauthorizer.h"
#include "json.h"
#include "pdebug.h"
#include <QUrl>
#include <QStringList>

using namespace Httpd;

AbstractController::AbstractController(HobrasoftHttpd::HttpConnection *parent) : HobrasoftHttpd::HttpRequestHandler(parent) {
    m_authorizer = NULL;
    m_connection = parent;
    m_checkId = false;
}


Db::Database *AbstractController::db() {
    return (authenticatedUser() != nullptr) 
        ? Db::Database::create(this, authenticatedUser())
        : Db::Database::create(this);
}


HobrasoftHttpd::HttpRequest *AbstractController::request() const { 
    return m_request; 
}


HobrasoftHttpd::HttpConnection *AbstractController::connection() const {
    return m_connection;
}


bool AbstractController::isAuthorized(Security::Permissions::Permission permission) {
    if (m_authorizer == NULL) {
        return true;
        }
    return m_authorizer->isAuthorized(permission);
}


void AbstractController::service(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    m_request = request;
    QString path = request->path();
    QStringList parts = path.split("/");

    if (parts.size() <= 1) {
        serviceError(request, response, 404, "not-found", "Object not found");
        return;
        }

    parts.removeFirst();
    if (parts.first() == "api") { parts.removeAt(0); }
    if (parts.first() == "v1") { parts.removeAt(0); }

    switch (parts.size()) {
        case 1:
            if (request->method() != "GET") {
                serviceError(request, response, 405, "bad-request", "Method not allowed");
                return;
                }
            serviceList(request, response);
            return;

        case 2:
            if (parts[1] == "events") {
                if (request->method() != "GET") {
                    serviceError(request, response, 405, "bad-request", "Method not allowed");
                    return;
                    }
                response->setHeader("Content-Type",      "text/event-stream");
                response->setHeader("Cache-Control",     "no-cache,public");
                response->setHeader("Transfer-Encoding", "chunked");
                if (!request->header("Referer").isEmpty()) {
                    QUrl referer = QUrl::fromUserInput(request->header("Referer"));
                    QUrl allow;
                    allow.setScheme(referer.scheme());
                    allow.setHost(referer.host());
                    allow.setPort(referer.port());
                    response->setHeader("Access-Control-Allow-Origin", allow.toString());
                    }
                response->flushSocket();
                serviceEvents(request, response);
                return;
                }

            if (parts[1] != "events") {
                QString id = parts[1];
                if (request->method() == "GET") {
                    if (!exists(request, id)) {
                        serviceError(request, response, 404, "not-found", "Not found");
                        return;
                        }
                    response->setHeader("Content-Type",  "application/json");
                    response->setHeader("Cache-Control", "no-cache,public");
                    serviceIdGet(request, response, id);
                    return;
                    }
                if (request->method() == "PUT") {

                    bool ok = false;
                    QVariantMap data = JSON::data(request->body(), &ok).toMap();
                    if (!ok) { 
                        serviceError(request, response, 400, "bad-request", "Could not parse JSON data");
                        return; 
                        }       

                    if (m_checkId && !data.contains("id")) {
                        serviceError(request, response, 400, "bad-request", "Data must contain ID");
                        return; 
                        } 

                    response->setHeader("Content-Type",  "application/json");
                    response->setHeader("Cache-Control", "no-cache,public");
                    serviceIdPut(request, response, data);
                    return;
                    }
                if (request->method() == "POST") {

                    bool ok = false;
                    QVariantMap data = JSON::data(request->body(), &ok).toMap();
                    if (!ok) { 
                        serviceError(request, response, 400, "bad-request", "Could not parse JSON data");
                        return; 
                        }       

                    if (m_checkId && !data.contains("id")) {
                        serviceError(request, response, 400, "bad-request", "Data must contain ID");
                        return; 
                        } 

                    response->setHeader("Content-Type",  "application/json");
                    response->setHeader("Cache-Control", "no-cache,public");
                    serviceIdPost(request, response, data);
                    return;
                    }
                if (request->method() == "DELETE") {
                    if (!exists(request, id)) {
                        serviceError(request, response, 404, "not-found", "Not found");
                        return;
                        }
                    response->setHeader("Content-Type",  "application/json");
                    response->setHeader("Cache-Control", "no-cache,public");
                    serviceIdDelete(request, response, id);
                    return;
                    }
                }
            return;

        case 3:
            if (parts[1] == "events" || parts[2] != "events") {
                serviceError(request, response, 404, "not-found", "Not found");
                return;
                }

            if (request->method() != "GET") {
                serviceError(request, response, 405, "bad-request", "Method not allowed");
                return;
                }

            if (!exists(request, parts[1])) {
                serviceError(request, response, 404, "not-found", "Not found");
                return;
                }

            response->setHeader("Content-Type",      "text/event-stream");
            response->setHeader("Cache-Control",     "no-cache,public");
            response->setHeader("Transfer-Encoding", "chunked");
            if (!request->header("Referer").isEmpty()) {
                QUrl referer = QUrl::fromUserInput(request->header("Referer"));
                QUrl allow;
                allow.setScheme(referer.scheme());
                allow.setHost(referer.host());
                allow.setPort(referer.port());
                response->setHeader("Access-Control-Allow-Origin", allow.toString());
                }
            response->flushSocket();
            serviceIdEvents(request, response, parts[1]);
            return;

        default:
            serviceError(request, response, 400, "bad-request", "Request could not be recognized");
            return;
        }
}


void AbstractController::serviceError(
        HobrasoftHttpd::HttpRequest *request, 
        HobrasoftHttpd::HttpResponse *response, 
        int code, 
        const QString& error, 
        const QString& reason) {

    Q_UNUSED(request);

    QVariantMap data;
    data["error"]  = error;
    data["reason"] = reason;

    HobrasoftHttpd::HttpResponse *r = (response == nullptr) ? new HobrasoftHttpd::HttpResponse(connection()) : response;
    r->clearHeaders();
    r->setHeader("Content-Type",  "application/json");
    r->setHeader("Cache-Control", "no-cache,public");
    r->setStatus(code, error);
    r->write( JSON::json(data) );
    r->flush();
//  connection()->deleteLater();
}


void AbstractController::serviceError(
        HobrasoftHttpd::HttpRequest *request, 
        HobrasoftHttpd::HttpResponse *response, 
        int code, 
        const QString& error, 
        const QVariantMap& data) {

    Q_UNUSED(request);

    HobrasoftHttpd::HttpResponse *r = (response == nullptr) ? new HobrasoftHttpd::HttpResponse(connection()) : response;
    r->clearHeaders();
    r->setHeader("Content-Type",  "application/json");
    r->setHeader("Cache-Control", "no-cache,public");
    r->setStatus(code, error);
    r->write( JSON::json(data) );
    r->flush();
//  connection()->deleteLater();
}


void AbstractController::serviceOK (
        HobrasoftHttpd::HttpRequest *request, 
        HobrasoftHttpd::HttpResponse *response, 
        const QVariant& data) {

    Q_UNUSED(request);

    QVariantMap okdata;
    okdata["ok"] = true;

    if (!response->containsHeader("Content-Type"))  { response->setHeader("Content-Type",  "application/json"); }
    if (!response->containsHeader("Cache-Control")) { response->setHeader("Cache-Control", "no-cache,public"); }
    response->setStatus(200, "OK");
    response->write( JSON::json( (!data.isValid() || data.isNull()) ? okdata : data) );
    response->flush();
}


void AbstractController::serviceEvent (
        HobrasoftHttpd::HttpRequest *request, 
        HobrasoftHttpd::HttpResponse *response, 
        const QVariant& data) { 
    Q_UNUSED(request);
    Q_UNUSED(response);

    // Pozor! Hlavička se neposílá, ale je potřeba
    // Skutečná hlavička se posílá v metodě service()
    HobrasoftHttpd::HttpResponse *lresponse = HttpRequestHandler::response();
    lresponse->setHeader("Transfer-Encoding", "chunked");
    lresponse->setHeader("Content-Type",      "text/event-stream");
    lresponse->setHeader("Cache-Control",     "no-cache,public");
    lresponse->setSendHeaders(false);
    
    QByteArray datagram;
    datagram += "event: status\n";
    datagram += "data: ";
    datagram += JSON::json(data);
    datagram += "\n";
    datagram += "\n";
    lresponse->write(datagram);
    lresponse->flushSocket();
    lresponse->flushAndDelete();

}

void AbstractController::serviceList      (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    serviceError(request, response, 501, "not-implemeted", "Method not implemented");
}


void AbstractController::serviceEvents    (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    serviceError(request, response, 501, "not-implemeted", "Method not implemented");
}


void AbstractController::serviceIdEvents  (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString&) {
    serviceError(request, response, 501, "not-implemeted", "Method not implemented");
}


void AbstractController::serviceIdDelete  (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString&) {
    serviceError(request, response, 501, "not-implemeted", "Method not implemented");
}


void AbstractController::serviceIdGet     (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString&) {
    serviceError(request, response, 501, "not-implemeted", "Method not implemented");
}


void AbstractController::serviceIdPut     (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap&) {
    serviceError(request, response, 501, "not-implemeted", "Method not implemented");
}


void AbstractController::serviceIdPost    (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap&) {
    serviceError(request, response, 501, "not-implemeted", "Method not implemented");
}

