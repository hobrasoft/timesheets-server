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
 * - @ref Httpd::ControllerTickets - /tickets - Manipuluje s informacemi o ticketech
 * @}
 */


namespace Httpd {

/**
@brief Manipuluje s informacemi o ticketech

Vrací seznam tiketů, které jsou vlastněné přihlášeným uživatelem, nebo 
spadají do kategorie, do které má uživatel přístup.

Implementované metody:
- get
- put, post
- delete

@param all - Normálně jsou v seznamu pouze otevřené tickety, parametrem _all = true_ lze vypsat všechny tickety, vždy však pouze tickety s přístupem přihlášeného uživatele

Struktura jednoho záznamu: 

@code
{
"category": 2,
"date": "2020-05-01T00:00:00.000",
"description": "Práce na Simul 6 - vývoj",
"price": 1000,
"ticket": 1,
"user": 2
}
@endcode


*/
class ControllerTickets : public AbstractController {
    Q_OBJECT
  public:
    ControllerTickets(HobrasoftHttpd::HttpConnection *parent);

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
