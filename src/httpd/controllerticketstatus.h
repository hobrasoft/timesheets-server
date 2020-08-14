/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerTicketStatus_H_
#define _Httpd_ControllerTicketStatus_H_

#include <QObject>
#include "abstractcontroller.h"



/**
 * @addtogroup apiurl
 * @{
 * - @ref Httpd::ControllerTicketStatus - /ticketstatus - Manipulace se stavy ticketů
 * @}
 */

namespace Httpd {

/**
@brief Manipulace se stavy ticketů

Každý ticket může obsahovat několik za sebou následujících stavů, například: nový, v řešení, vyřešeno, vyfakturováno, zavřeno.
Při změně celého ticketu může být vhodnější použít @ref Httpd::ControllerTicketsVw

Implementované metody:

- get
- put, post
- del

@param all - Normálně jsou v seznamu pouze otevřené tickety, parametrem _all = true_ lze vypsat všechny tickety, vždy však pouze tickety s přístupem přihlášeného uživatele

Struktura jednoho záznamu:
@code
{
"id": 9,                                // Primární klíč, při insertu se může změnit
"date": "2020-06-01T00:00:00.000",      // datum zavedení
"status": "NEW",                        // Status, klíč v tabulce statuses
"description": "Práce zahájeny",        // Popis
"ticket": 2,                            // Ticket, klíč v tabulce tickets
"user": 2,                              // Uživatel, klíč v tabulce users
}
@endcode

*/
class ControllerTicketStatus : public AbstractController {
    Q_OBJECT
  public:
    ControllerTicketStatus(HobrasoftHttpd::HttpConnection *parent);

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
