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
@endcode

*/
class ControllerStatusOrder : public AbstractController {
    Q_OBJECT
  public:
    ControllerStatusOrder(HobrasoftHttpd::HttpConnection *parent);

  protected:

    virtual void serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response);

  private slots:

  private:

};

}

#endif
