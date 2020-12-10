/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerStatusTemplates_H_
#define _Httpd_ControllerStatusTemplates_H_

#include <QObject>
#include "abstractcontroller.h"



/**
 * @addtogroup apiurl
 * @{
 * - @ref Httpd::ControllerStatusTemplates - /statustemplates - Manipulace se šablonami stavů
 * @}
 */


namespace Httpd {

/**
@brief Manipulace se šablonami stavů

Implementuje metody:
- get

Struktura jednoho záznamu
@code
{
"id": 11,                       // Primární klíč
"status": "NEW"                 // Ke kterému stavu se vztahuje
"code": "23409"                 // Kód (zkratka)
"category": "12-23"             // Ke které kategorii se vztahuje
"title": "Nový",                // Popis stavu, v jazyce uživatele
"description": "{cs:'abcd'}",   // Podrobný popis v JSON, více jazyků, uživatel obvykle nevidí
}

@endcode


*/
class ControllerStatusTemplates : public AbstractController {
    Q_OBJECT
  public:
    ControllerStatusTemplates(HobrasoftHttpd::HttpConnection *parent);

  protected:

    void serviceList     (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) Q_DECL_OVERRIDE;
    void serviceIdGet    (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) Q_DECL_OVERRIDE;

  private slots:

  private:

};

}

#endif
