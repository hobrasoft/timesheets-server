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
#include "version.h"
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
    m_authenticatedUser = nullptr;
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

    m_authenticatedUser = new AuthenticatedUser(this);

    if (!m_authenticatedUser->authenticate(user, password)) {
        response->setStatus(401, "Unauthorized");
        response->flush();
        m_authenticatedUser->setAuthenticated(false);
        return false;
        }

    if (request->path() == "/api/v1/authenticate") {
        session.add("user", user);
        session.add("password", password);
        QVariantMap data;
        data["userid"]             = m_authenticatedUser->user();
        data["username"]           = m_authenticatedUser->login();
        data["name"]               = m_authenticatedUser->name();
        data["server_name"]        = MSETTINGS->serverName();
        data["server_description"] = MSETTINGS->serverDescription();
        data["server_git_commit"]  = GIT_COMMIT;
        data["server_git_branch"]  = GIT_BRANCH;
        data["server_version"]     = VERSION;
        data["server_qt_version"]  = qtVersion();

        Db::Database *db = Db::Database::create(this);
        QList<Dbt::ClientSettings> cslist = db->clientSettings();
        if (cslist.size() == 1) {
            const Dbt::ClientSettings& cs = cslist[0];
            data["multiple_timesheets"]      = cs.multiple_timesheets;
            data["show_price"]               = cs.show_price;
            data["can_change_category"]      = cs.can_change_category;
            data["edit_categories"]          = cs.edit_categories;
            data["show_multiple_timesheets"] = cs.show_multiple_timesheets;
            data["show_show_price"]          = cs.show_show_price;
            data["show_can_change_category"] = cs.show_can_change_category;
            data["show_edit_categories"]     = cs.show_edit_categories;
            }
        db->deleteLater();

        response->setStatus(200, "OK");
        response->setHeader("Content-Type",  "application/json");
        response->setHeader("Cache-Control", "no-cache,public");
        response->write(JSON::json(data));
        response->flush();
        return false; // ano, false!
        }

    return true;

}


bool RequestAuthorizer::isAuthorized(Security::Permissions::Permission permission) const {
    return GUARD->isGranted(permission, role());
}


Security::Roles::Role RequestAuthorizer::role() const {
    return Security::Roles::User;
}



