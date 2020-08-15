/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerStatusOrder_H_
#define _Httpd_ControllerStatusOrder_H_

#include <QObject>
#include "abstractcontroller.h"

/**
 * @addtogroup apiurl
 * @{
 * - @ref Httpd::ControllerStatusOrder - /statusorder - vrací informace o pořadí přechodů mezi jednotlivými stavy ticketu
 * @}
 */


namespace Httpd {

/**
@brief Vrací informace o pořadí přechodů mezi jednotlivými stavy ticketu

Implementuje metody:
- get
- put, post
- delete

Struktura jednoho záznamu:

@code
{
"id": 12,
"category": null,
"next_status": "CLOSED",
"previous_status": "NEW"
}
@endcode

*/
class ControllerStatusOrder : public AbstractController {
    Q_OBJECT
  public:
    ControllerStatusOrder(HobrasoftHttpd::HttpConnection *parent);

  protected:

    void serviceList     (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) Q_DECL_OVERRIDE;
    void serviceIdGet    (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& data) Q_DECL_OVERRIDE;
    void serviceIdPut    (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) Q_DECL_OVERRIDE;
    void serviceIdPost   (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) Q_DECL_OVERRIDE;
    void serviceIdDelete (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) Q_DECL_OVERRIDE;


  private slots:

  private:

};

}

#endif
