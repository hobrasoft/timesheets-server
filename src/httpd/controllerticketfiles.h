/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerTicketFiles_H_
#define _Httpd_ControllerTicketFiles_H_

#include <QObject>
#include "abstractcontroller.h"



/**
 * @addtogroup apiurl
 * @{
 * - @ref Httpd::ControllerTicketFiles - /ticketfiles - Manipulace se soubory ticketů
 * @}
 */

namespace Httpd {

/**
@brief Manipulace se soubory ticketů

Implementované metody:
- get
- put, post
- del

@param ticket - Vybere záznamy k zadanému ticketu, na konci URL musí být zadané ID alespoň ve formě lomítka (ignoruje se)
@param all - Normálně jsou v seznamu pouze otevřené tickety, parametrem _all = true_ lze vypsat všechny tickety, vždy však pouze tickety s přístupem přihlášeného uživatele

Struktura jednoho záznamu:
@code
{
"id": 9,                                // Primární klíč, při insertu se může změnit
"user": 2,                              // Uživatel, klíč v tabulce users
"date": "2020-06-01T00:00:00.000",      // Datum zavedení
"ticket": 2,                            // Ticket, klíč v tabulce tickets
"name":"logo.png",                      // Jméno souboru
"type":"image/png","user":1,            // Content-Typy soubor 
"content":"iVBORw0KGgodr9ncLps2nfX..."  // Obsah, base64
}
@endcode


*/
class ControllerTicketFiles : public AbstractController {
    Q_OBJECT
  public:
    ControllerTicketFiles(HobrasoftHttpd::HttpConnection *parent);

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
