/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerTicketTimesheets_H_
#define _Httpd_ControllerTicketTimesheets_H_

#include <QObject>
#include "abstractcontroller.h"



/**
 * @addtogroup apiurl
 * @{
 * - @ref Httpd::ControllerTicketTimesheets - /tickettimesheets - Manipulace se stavy ticketů
 * @}
 */

namespace Httpd {

/**
@brief Manipulace se záznamy pracovního výkazu

Při změně celého ticketu může být vhodnější použít @ref Httpd::ControllerTicketsVw

Na server by se měly posílat záznamy s nastaveným date_from i date_to.

Implementuje metody:
- get
- put, post
- delete

Struktura jednoho záznamu:

@code
{
"id": 34,                                   // ID záznamu, primární klíč, může se při insertu změnit
"user": 2,                                  // ID uživatele, kterému záznam patří
"ticket": 1,                                // ID ticketu
"created": false,                           // Směr server->klient vždy false, opačně jde o záznam, který se má vložit = dostane nové ID
"modified": false,                          // Směr server->klient vždy false, opačně jde o záznam, který se na klientovi změnil
"date_from": "2020-08-13T15:12:25.619",     // Začátek pracovního úkonu
"date_to": "2020-08-13T15:12:53.198"        // Konec pracovního úkonu
}
@endcode

*/
class ControllerTicketTimesheets : public AbstractController {
    Q_OBJECT
  public:
    ControllerTicketTimesheets(HobrasoftHttpd::HttpConnection *parent);

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
