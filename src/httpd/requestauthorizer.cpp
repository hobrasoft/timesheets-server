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
    m_adminuser      = MSETTINGS->httpdUser();
    m_adminpassword  = MSETTINGS->httpdPassword();
    m_authorize      = MSETTINGS->httpdAuthorize();
}


bool RequestAuthorizer::isLoggedIn(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    HttpSession session = SessionStore::sessionStore()->session(request, response);

    if (request->path() == "/login.html") {
        SessionStore::sessionStore()->remove(session);
        }

    QString user      = request->parameter("user");
    QString password  = request->parameter("password");

    if (!user.isEmpty() && !password.isEmpty()) {
        session.add("user", user);
        session.add("password", password);
        }

    user = session.value("user").toString();
    password = session.value("password").toString();
    QString userX     = (user == m_adminuser) ? m_adminuser : user;
    QString passwordX = (user == m_adminuser) ? m_adminpassword : "aaaaaaaa";

    /**
     * Udržuje si informace o session, ukládá sem informace user a password
     * a při každém požadavku je kontroluje. Pokud jméno a heslo nesedí, 
     * přesměruje na /login.html a odtud na /authorize
     */
    if (request->path() == "/authorize") {
        if (user != userX || password != passwordX ) {
            response->setStatus(302, "Found");
            response->setHeader("Location", "login.html");
            response->write("302 Found");
            response->flush();
            return false;
            }
        session.add("user", user);
        session.add("password", password);
        response->setStatus(302, "Found");
        response->setHeader("Location", "/html/index.shtml");
        response->write("302 Found");
        response->flush();
        return false;
        }

    if (m_authorize && 
         (session.value("user").toString() != userX || 
          session.value("password").toString() != passwordX) &&
         !request->path().endsWith(".css") &&
          request->path() != "/login.html" ) {
        // redirect na login stránku
        int pocetPodadresaru = request->path().split("/", QString::SkipEmptyParts).size();
        QString cesta;
        if (pocetPodadresaru == 2) { cesta = "../"; }
        if (pocetPodadresaru == 3) { cesta = "../../"; }
        if (pocetPodadresaru == 4) { cesta = "../../../"; }
        response->setStatus(302, "Found");
        response->setHeader("Location", cesta+"login.html");
        response->write("302 Found");
        response->flush();
        return false;
        }

    m_user = session.value("user").toString();

    PDEBUG << user << password << "overeno";
    return true;

}


bool RequestAuthorizer::isAuthorized(Security::Permissions::Permission permission) const {
    return GUARD->isGranted(permission, role());
}


Security::Roles::Role RequestAuthorizer::role() const {
    if (m_user == m_adminuser) {
        return Security::Roles::Admin;
        }

    if (!m_authorize) {
        return Security::Roles::Admin;
        }

    return Security::Roles::User;
}



