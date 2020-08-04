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
 * - @ref Httpd::ControllerStatusOrder - vrací informace o pořadí přechodů mezi jednotlivými stavy ticketu
 * @}
 */


namespace Httpd {

/**
@brief Vrací informace o pořadí přechodů mezi jednotlivými stavy ticketu

@code
[
  {
    "category": null,
    "next_status": "CLOSED",
    "previous_status": "NEW"
  },
  {
    "category": null,
    "next_status": "ARCHIVE",
    "previous_status": "NEW"
  },
  {
    "category": null,
    "next_status": "ARCHIVE",
    "previous_status": "CLOSED"
  },
  {
    "category": 5,
    "next_status": "PRIORITY",
    "previous_status": "NEW"
  },
  {
    "category": 5,
    "next_status": "CLOSED",
    "previous_status": "PRIORITY"
  },
  {
    "category": 5,
    "next_status": "ARCHIVE",
    "previous_status": "PRIORITY"
  }
]
@endcode

*/
class ControllerStatusOrder : public AbstractController {
    Q_OBJECT
  public:
    ControllerStatusOrder(HobrasoftHttpd::HttpConnection *parent);

  protected:

    void serviceList     (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) Q_DECL_OVERRIDE;
    void serviceIdPut    (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) Q_DECL_OVERRIDE;
    void serviceIdPost   (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) Q_DECL_OVERRIDE;
    void serviceIdDelete (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) Q_DECL_OVERRIDE;


  private slots:

  private:

};

}

#endif
