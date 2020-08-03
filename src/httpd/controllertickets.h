/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerTickets_H_
#define _Httpd_ControllerTickets_H_

#include <QObject>
#include "abstractcontroller.h"


/**
 * @addtogroup apiurl
 * @{
 * - @ref Httpd::ControllerTickets - Manipuluje s informacemi o ticketech
 * @}
 */


namespace Httpd {

/**
@brief Manipuluje s informacemi o ticketech

GET /tickets
---
Vrací seznam tiketů, které jsou vlastněné přihlášeným uživatelem, nebo 
spadají do kategorie, do které má uživatel přístup.

@param all - Normálně jsou v seznamu pouze otevřené tickety, parametrem _all = true_ lze vypsat všechny tickety

@code
[
  {
    "category": 2,
    "date": "2020-05-01T00:00:00.000",
    "description": "Práce na Simul 6 - vývoj",
    "price": 1000,
    "ticket": 1,
    "user": 2
  },
  {
    "category": 2,
    "date": "2020-05-01T00:00:00.000",
    "description": "Práce na Simul 6 - opravy",
    "price": 1000,
    "ticket": 2,
    "user": 2
  }
]
@endcode

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
