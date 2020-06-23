/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "requestauthorizer.h"
#include "httprequesthandler.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "httpconnection.h"
#include "httpsession.h"
#include "msettings.h"
#include "sessionstore.h"
#include "security/guard.h"
#include "security/roles.h"
#include "db.h"
#include "json.h"
#include "pdebug.h"


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


RequestAuthorizer::RequestAuthorizer(HobrasoftHttpd::HttpConnection *parent) : HobrasoftHttpd::HttpRequestHandler(parent) {
}


bool RequestAuthorizer::isLoggedIn(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    HttpSession session = SessionStore::sessionStore()->session(request, response);

    if (request->path() == "/unatuhenticate") {
        SessionStore::sessionStore()->remove(session);
        response->setStatus(204, "OK");
        response->flush();
        return false;
        }

    if (!request->parameter("user").isEmpty() && !request->parameter("password").isEmpty()) {
        session.add("user",     request->parameter("user"));
        session.add("password", request->parameter("password"));
        }

    QString user     = session.value("user").toString();
    QString password = session.value("password").toString();

    QList<Dbt::Users> ulist = DB->authenticate(user, password);
    // PDEBUG << user << password << "ulist.isEmpty()" << ulist.isEmpty();
    if (ulist.isEmpty()) {
        response->setStatus(401, "Unauthorized");
        response->flush();
        return false;
        }

    if (request->path() == "/api/v1/authenticate") {
        session.add("user", user);
        session.add("password", password);
        QVariantMap data;
        data["userid"] = ulist[0].user;
        data["username"] = ulist[0].login;
        data["name"] = ulist[0].name;
        data["role"] = "Spravuje fše";
        response->setStatus(200, "OK");
        response->setHeader("Content-Type",  "application/json");
        response->setHeader("Cache-Control", "no-cache,public");
        response->write(JSON::json(data));
        response->flush();
        return false; // ano, false!
        }

    m_user = session.value("user").toString();

    return true;

}


bool RequestAuthorizer::isAuthorized(Security::Permissions::Permission permission) const {
    return GUARD->isGranted(permission, role());
}


Security::Roles::Role RequestAuthorizer::role() const {
    return Security::Roles::User;
}



