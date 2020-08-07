/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerTicketsVw_H_
#define _Httpd_ControllerTicketsVw_H_

#include <QObject>
#include "abstractcontroller.h"


/**
 * @addtogroup apiurl
 * @{
 * - @ref Httpd::ControllerTicketsVw - Manipuluje s informacemi o ticketech
 * @}
 */


namespace Httpd {

/**
@brief Manipuluje s informacemi o ticketech

GET /tickets
---
Vrací seznam tiketů, které jsou vlastněné přihlášeným uživatelem, nebo 
spadají do kategorie, do které má uživatel přístup.

Přípustné modifikace URL:
- /tickets - vrací pole ticketů
- /tickets/<id> - vrací jeden ticket

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
class ControllerTicketsVw : public AbstractController {
    Q_OBJECT
  public:
    ControllerTicketsVw(HobrasoftHttpd::HttpConnection *parent);

  protected:

    void serviceList  (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) Q_DECL_OVERRIDE;
    void serviceIdGet (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) Q_DECL_OVERRIDE;
    void serviceIdPut    (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) Q_DECL_OVERRIDE;
    void serviceIdPost   (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) Q_DECL_OVERRIDE;
    void serviceIdDelete (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) Q_DECL_OVERRIDE;

  private slots:

  private:

};

}

#endif
