/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerEmployees_H_
#define _Httpd_ControllerEmployees_H_

#include <QObject>
#include "abstractcontroller.h"


/**
 * @addtogroup apiurl
 * @{
 * - @ref Httpd::ControllerEmployees - /employees - Manipuluje s informacemi o zaměstnancích v docházce
 * @}
 */


namespace Httpd {

/**
@brief Manipuluje s informacemi o zaměstnancích v docházce

Vrací seznam oddělení v docházce.

Implementované metody:
- get
- put, post
- delete

Struktura jednoho záznamu: 

@code
{
"department": 23,
"abbr": "VY",
"description": "Výroba"
}
@endcode


*/
class ControllerEmployees : public AbstractController {
    Q_OBJECT
  public:
    ControllerEmployees(HobrasoftHttpd::HttpConnection *parent);

  protected:

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
