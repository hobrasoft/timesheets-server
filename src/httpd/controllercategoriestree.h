/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerCategoriesTree_H_
#define _Httpd_ControllerCategoriesTree_H_

#include <QObject>
#include "abstractcontroller.h"

/**
 * @addtogroup apiurl
 * @{
 * - @ref Httpd::ControllerCategoriesTree - /categoriestree - Výpis stromu kategorií
 * @}
 */



namespace Httpd {

/**
@brief Výpis stromu kategorií

Je-li zadané id, vypíše všechny kategorie, které mají jak rodiče toto id.
Není-li zadané id, vypíše všechny kategorie od vrcholku stromu. 
Zadaná kategorie (rodičovská) ve výpisu není!

Implementuje metody:
- get

Struktura jednoho záznamu:
@code
{
"category": "1",                        // Primární klíč - identifikátor kategorie
"parent_category": "0",                 // Rodičovská kategorie
"description": "Projekty Hobrasoft",    // Popis kategorie
"price": 700,                           // Cena, která by se měla nastavit pro každý nový ticket v této kategorii
"categories": { {....podkategorie } ]
}
@endcode

*/
class ControllerCategoriesTree : public AbstractController {
    Q_OBJECT
  public:
    ControllerCategoriesTree(HobrasoftHttpd::HttpConnection *parent);

  protected:

    void serviceList     (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) Q_DECL_OVERRIDE;
    void serviceIdGet    (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) Q_DECL_OVERRIDE;

  private slots:

  private:
    QVariantList subtree(const QString& id);

};

}

#endif
