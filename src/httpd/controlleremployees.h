/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerEmployees_H_
#define _Httpd_ControllerEmployees_H_

#include <QObject>
#include "abstractcontroller.h"


/**
 * @addtogroup apiurl
 * @{
 * - @ref Httpd::ControllerEmployees - /employees - Manipuluje s informacemi o zaměstnancích v docházce
 * @}
 */


namespace Httpd {

/**
@brief Manipuluje s informacemi o zaměstnancích v docházce

Vrací seznam oddělení v docházce.

Implementované metody:
- get
- put, post
- delete

Struktura jednoho záznamu:

@code
{
    "employee":         1234,
    "firstname":        "Petr",
    "surname":          "Hobrasoft",
    "active":           true,
    "user":             1,
    "work_hours_mode":  "full",
    "rounding_interval": "15m",
    "saturdays_paid":   true,
    "sundays_paid":     false,
    "auto_breaks":      false,
    "overtime_paid":    true,
    // Připojeno z jiných tabulek
    "departments":  [
        {"department":23, "abbr":"VY", "description":"Výroba"}, ...
        ],
    "rfids": [
        {"rfid":12, "rfid_id":"ABCD1234", "valid":true, "note":"Poznámka"}, ...
        ],
    "doors": [
        {"door":1, "description":"Výrobní dílna"}, ...
        ],
    "manages_departments": [
        {"department":23, "abbr":"VY", "description":"Výroba"}, ...
        ]
}
@endcode


*/
class ControllerEmployees : public AbstractController {
    Q_OBJECT
  public:
    ControllerEmployees(HobrasoftHttpd::HttpConnection *parent);

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
