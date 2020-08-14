/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerTicketValues_H_
#define _Httpd_ControllerTicketValues_H_

#include <QObject>
#include "abstractcontroller.h"



/**
 * @addtogroup apiurl
 * @{
 * - @ref Httpd::ControllerTicketValues - /ticketvalues - Manipulace se různými uživatelskými objekty ticketů
 * @}
 */

namespace Httpd {

/**
@brief Manipulace s přibalenými objekty ticketů

Každý ticket může obsahovat několik různých (JSON) objektů. V aplikaci je takový objekt identifikovaný svým jménem.
Obsahovat může hodnoty jako například GPS souřadnice, obsah RFID tagů a podobně.

Při změně celého ticketu může být vhodnější použít @ref Httpd::ControllerTicketsVw

Implementované metody:

- get
- put, post
- del

@param all - Normálně jsou v seznamu pouze otevřené tickety, parametrem _all = true_ lze vypsat všechny tickety, vždy však pouze tickety s přístupem přihlášeného uživatele

Struktura jednoho záznamu:

@code

@endcode

*/
class ControllerTicketValues : public AbstractController {
    Q_OBJECT
  public:
    ControllerTicketValues(HobrasoftHttpd::HttpConnection *parent);

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
