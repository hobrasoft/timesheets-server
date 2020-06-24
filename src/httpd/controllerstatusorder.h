/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerStatusOrder_H_
#define _Httpd_ControllerStatusOrder_H_

#include <QObject>
#include "abstractcontroller.h"

namespace Httpd {

/**
 * @brief Vyřizuje požadavky /server - informace a příkazy k fungování serveru (restart, aktivace a podobně)
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
