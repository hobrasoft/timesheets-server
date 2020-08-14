/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerCategories_H_
#define _Httpd_ControllerCategories_H_

#include <QObject>
#include "abstractcontroller.h"

/**
 * @addtogroup apiurl
 * @{
 * - @ref Httpd::ControllerCategories - /categories - Manipulace s kategoriemi
 * @}
 */



namespace Httpd {

/**
@brief Manipulace s kategoriemi

Kategorie se v programu řadí ve stromové struktuře. 
V každé kategorii může být libovolný počet ticketů.
Ke kategoriím se nastavují přístupová práva na uživatele (@ref Httpd::ControllerUsersCategories).

Implementuje metody:
- get
- post, put
- delete

Primární klíč je v API předávaný jako řetězec (nutné pro fotomon), ale v modulu postgresql je uložené ve formátu integer.

Struktura jednoho záznamu:
@code
{
"category": "1",                        // Primární klíč - identifikátor kategorie
"parent_category": "0",                 // Rodičovská kategorie
"description": "Projekty Hobrasoft",    // Popis kategorie
"price": 700                            // Cena, která by se měla nastavit pro každý nový ticket v této kategorii
}
@endcode

*/
class ControllerCategories : public AbstractController {
    Q_OBJECT
  public:
    ControllerCategories(HobrasoftHttpd::HttpConnection *parent);

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
