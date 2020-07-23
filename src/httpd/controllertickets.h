/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerTickets_H_
#define _Httpd_ControllerTickets_H_

#include <QObject>
#include "abstractcontroller.h"

namespace Httpd {

/**
 * @brief Vyřizuje požadavky /server - informace a příkazy k fungování serveru (restart, aktivace a podobně)
 */
class ControllerTickets : public AbstractController {
    Q_OBJECT
  public:
    ControllerTickets(HobrasoftHttpd::HttpConnection *parent);

  protected:

    virtual void serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response);

  private slots:

  private:

};

}

#endif
