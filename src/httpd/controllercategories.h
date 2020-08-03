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
Ke kategoriím se nastavují přístupová práva na uživatele.

GET 
---
Vrací seznam všech kategorií, ke kterým má uživatel přístup.
@code
[
  {
    "category": "1",                        // Primární klíč - identifikátor kategorie
    "parent_category": "0",                 // Rodičovská kategorie
    "description": "Projekty Hobrasoft",    // Popis kategorie
    "price": 700                            // Cena, která by se měla nastavit pro každý nový ticket v této kategorii
  },
  {
    "category": "2",
    "parent_category": "1",
    "description": "Projekty Petr Bravenec",
    "price": 700
  },
  {
    "category": "4",
    "parent_category": "2",
    "description": "Simul 6",
    "price": 1000
  },
  {
    "category": "5",
    "parent_category": "3",
    "description": "Timesheets",
    "price": 700
  }
]
@endcode

*/
class ControllerCategories : public AbstractController {
    Q_OBJECT
  public:
    ControllerCategories(HobrasoftHttpd::HttpConnection *parent);

  protected:

    virtual void serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response);

  private slots:

  private:

};

}

#endif
