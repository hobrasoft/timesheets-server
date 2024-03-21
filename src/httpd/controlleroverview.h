/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerOverview_H_
#define _Httpd_ControllerOverview_H_

#include <QObject>
#include "abstractcontroller.h"


/**
 * @addtogroup apiurl
 * @{
 * - @ref Httpd::ControllerOverview - /ticketsvw - Manipuluje s informacemi o ticketech
 * @}
 */


namespace Httpd {

/**
@brief Vrací podklady pro tištěný pracovní výkaz

GET /overview/&lt;categoryid&gt;
---

@param statuses - seznam statutů, které se mají ve výkazu objevit, například: statuses=NEW,INVOICE

Nerozbalený JSON vypadá takto:
@code
{
  // informace o vybrané kategorii
  category: {
    category: "10",
    description: "Fotomon vývoj - fotomon",
    parent_category: "2",
    price: 700
  },
  // informace po jednotlivých dnech, "group by date, user"
  days: {
    records: [...]
    sum: { duration: 1235, price: 2344 }
    },
  // informace po jednotlivých ticketech, "group by ticket, user"
  tickets: {
    records: [...],     // group by ticket, user
    sum: [...}          // group by ticket
    }
}
@endcode

Rozbalený JSON:
@code
{
  "category": {
    "category": "10",
    "description": "Fotomon vývoj - fotomon",
    "parent_category": "2",
    "price": 700
  },
  "days": {
    "records": [
      {
        "date": "2020-04-07T00:00:00.000",
        "description": "Fotomon server – rozšíření pro fotomoncontrol",
        "duration": 0,
        "hour_price": 0,
        "price": 3,
        "ticket": 37,
        "user": 2,
        "user_name": "700"
      }, ...
  ],
  "sum": {
      "duration": 152132,
      "price": 0
    }
  },
  "tickets": {
    "records": [
      {
        "description": "Bayes vyhodnocení závady elektrárny",
        "duration": 0,
        "hour_price": 700,
        "price": 0.083333336,
        "ticket": 45,
        "user": 2,
        "user_name": "Petr Bravenec"
      }, ...
    ],
    "sum": [
      {
        "description": "Ticketing",
        "duration": 146.33333,
        "price": 102433.32977294922,
        "ticket": 36
      },...
    ]
  }
}
@endcode


*/
class ControllerOverview : public AbstractController {
    Q_OBJECT
  public:
    ControllerOverview(HobrasoftHttpd::HttpConnection *parent);

  protected:

    void serviceList  (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) Q_DECL_OVERRIDE;
    void serviceIdGet (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) Q_DECL_OVERRIDE;

  private slots:

  private:

};

}

#endif
