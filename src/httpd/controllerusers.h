/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_Users_H_
#define _Httpd_Users_H_

#include <QObject>
#include "abstractcontroller.h"



/**
 * @addtogroup apiurl
 * @{
 * - @ref Httpd::ControllerUsers - /users - Manipulace s uživateli
 * @}
 */


namespace Httpd {

/**
@brief Manipulace s uživateli

Implementuje metody:
- get
- post, put
- delete

Uživatele může modifikovat pouze přihlášený uživatel s právy administrátora.


Struktura jednoho záznamu:
@code
{
"admin": true,
"enabled": true,
"lang": "en",
"login": "admin",
"name": "Administrator",
"user": 1
}
@endcode

*/
class ControllerUsers : public AbstractController {
    Q_OBJECT
  public:
    ControllerUsers(HobrasoftHttpd::HttpConnection *parent);

  protected:

    void service         (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) Q_DECL_OVERRIDE;
    void serviceList     (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) Q_DECL_OVERRIDE;
    void serviceIdGet    (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) Q_DECL_OVERRIDE;
    void serviceIdPut    (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) Q_DECL_OVERRIDE;
    void serviceIdPost   (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) Q_DECL_OVERRIDE;
    void serviceIdDelete (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) Q_DECL_OVERRIDE;

  private slots:

  private:

};

}

#endif
