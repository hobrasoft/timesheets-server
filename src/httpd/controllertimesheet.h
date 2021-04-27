/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerTimesheet_H_
#define _Httpd_ControllerTimesheet_H_

#include <QObject>
#include "abstractcontroller.h"



/**
 * @addtogroup apiurl
 * @{
 * - @ref Httpd::ControllerTimesheet - /timesheet - start, stop a toggle 
 * @}
 */

namespace Httpd {

/**
@brief Nastartování nebo zastavení pracovního výkazu

Na server se posílá příkaz (start, stop, toggle) a id ticketu, který se má spustit:

@code
/api/v1/timesheet/start/<id>
@endcode

Běžící výkazy lze zastavit hromadně jediným příkazem. Start a Toggle takto ovládat nelze!
Chceme-li zastavit všechny běžící pracovní výkazy, je možné použít příkaz:

@code
/api/v1/timesheet/stop/all
@endcode


Implementuje metody:
- get


Vrací jednoduchou informaci:

@code
[{
"id": 34,                                   // ID záznamu, primární klíč
"user": 2,                                  // ID uživatele, kterému záznam patří
"ticket": 1,                                // ID ticketu
"date_from": "2020-08-13T15:12:25.619",     // Začátek pracovního úkonu
"date_to": null                             // Konec pracovního úkonu, pokud ticket běží, vrací null
}]
@endcode

Příklady:

Nastartování zastaveného ticketu s id 111 (stejný efekt by měl u zastaveného ticketu příkaz toggle):

/api/v1/timesheet/start/1
@code
[{
"ticket": 1111, 
"id": 34, 
"user": 2, 
"date_from": "2020-10-26T07:22:00",
"date_to": null                     // Běží, není ukončeno
}]
@endcode


Zastavení běžícího ticketu s id 111 (stejný efekt by měl u běžícího ticketu příkaz toggle):

/api/v1/timesheet/stop/1
@code
[{
"ticket": 1111, 
"id": 34, 
"user": 2, 
"date_from": "2020-10-26T07:22:00",
"date_to": "2020-10-26T07:24:12"    // Neběží, je ukončeno
}]
@endcode


Další nastartování zastaveného ticketu s id 111, id záznamu v tabulce timesheets má novou hodnotu,
tj. v tabulce přibyl nový záznam:

/api/v1/timesheet/start/1
@code
[{
"ticket": 1111, 
"id": 35,                           // Nový záznam
"user": 2, 
"date_from": "2020-10-26T07:22:00",
"date_to": null                     // Běží, není ukončeno
}]
@endcode

Pokud dojde k pokusu nastartovat běžící záznam, nebo k pokusu ukončit zastavený záznam, vrací chybu, HTTP status 409 Conflict.
Chyba může znamenat, že ticket je spuštěný z jiného klienta. U příkazu toggle k chybě nedochází, může ale dojít ke konfliktům
mezi různými klienty:

/api/v1/start/1
@code
[{
"ticket": 1111, 
"id": 35,                           // Nový záznam
"ok": false,
"error": "running"
"reason": "The ticket is running already"
}]
@endcode

*/
class ControllerTimesheet : public AbstractController {
    Q_OBJECT
  public:
    ControllerTimesheet(HobrasoftHttpd::HttpConnection *parent);

  protected:

    void service (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) Q_DECL_OVERRIDE;

  private slots:

  private:
    void serviceStart  (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, int id);
    void serviceStop   (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, int id);
    void serviceToggle (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, int id);

};

}

#endif
