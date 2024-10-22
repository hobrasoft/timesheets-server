/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerDoors_H_
#define _Httpd_ControllerDoors_H_

#include <QObject>
#include "abstractcontroller.h"


/**
 * @addtogroup apiurl
 * @{
 * - @ref Httpd::ControllerDoors - /door - Manipuluje s informacemi o dveřích v docházce
 * @}
 */


namespace Httpd {

/**
@brief Manipuluje s informacemi o dveřích v docházce

Vrací seznam dveří v docházce.

Implementované metody:
- get
- put, post
- delete

Struktura jednoho záznamu: 

@code
{
"door": 23,
"description": "Výroba"
}
@endcode


*/
class ControllerDoors : public AbstractController {
    Q_OBJECT
  public:
    ControllerDoors(HobrasoftHttpd::HttpConnection *parent);

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
