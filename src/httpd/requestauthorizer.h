/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_RequestAuthorizer_H_
#define _Httpd_RequestAuthorizer_H_

#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantMap>
#include "httprequesthandler.h"
#include "security/roles.h"
#include "security/permissions.h"
#include "authenticateduser.h"

namespace HobrasoftHttpd {
class HttpRequestHandler;
class HttpRequest;
class HttpResponse;
class HttpConnection;
}

namespace Httpd {

/**
 * @brief Kontroluje přihlašování uživatelů a jejich oprávnění přes webové rozhraní
 */
class RequestAuthorizer : public HobrasoftHttpd::HttpRequestHandler {
    Q_OBJECT
  public:

    /**
     * @brief Konstruktor, načítá konfiguraci z konfiguračního souboru
     */
    RequestAuthorizer(HobrasoftHttpd::HttpConnection *parent);

    /**
     * @brief Řídí přihlašování k webovému rozhraní
     *
     * @returns V případě, že je uživatel přihlášený, vrací true - je možné 
     *      pokračovat ve zpracování požadavku.
     *      V případě, že uživatel přihlášený není, vyvolá akce potřebné pro přihlášení
     *      (přesměrování na login stránku) a vrátí false - v takovém případě
     *      se je požadavek vyřízený a webový server nesmí ve zpracování pokračovat.
     */
    bool isLoggedIn(HobrasoftHttpd::HttpRequest *, HobrasoftHttpd::HttpResponse *);

    /**
     * @brief Vrací true, pokud má přihlášený uživatel požadované oprávnění
     */
    bool isAuthorized(Security::Permissions::Permission) const;

    /** 
     * Vrací roli přihlášeného uživatele
     *
     * V současnosti se rozlišují dvě role: Admin a User. Adminem je uživatel, jehož jméno
     * a heslo je uvedené v konfiguračním souborů. Uživatel admin může být jen jeden.
     * Uživatelskou roli dostávají všichni uživatele vytvoření přes webové rozhraní
     * a uložení v databází.
     *
     * Pokud je v konfiguračním souboru zakázaná autorizace požadavků, může k serveru
     * přes webové rozhraní přistupovat kdokoliv bez přihlášení s právy administrátora.
     */
    Security::Roles::Role role() const;

    AuthenticatedUser *authenticatedUser() const { return m_authenticatedUser; }

  protected:

  private:
    QString     m_adminuser;      ///< Uživatelské jméno pro přihlášení, bere se z konfigurace
    QString     m_adminpassword;  ///< Uživatelské heslo pro přihlášení, bere se z konfigurace 
    bool        m_authorize;      ///< Jestli se má přihlašovat, bere se z konfigurace
    AuthenticatedUser *m_authenticatedUser;

};

}

#endif
