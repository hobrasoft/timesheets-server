/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerDepartmentHasManager_H_
#define _Httpd_ControllerDepartmentHasManager_H_

#include <QObject>
#include "abstractcontroller.h"


/**
 * @addtogroup apiurl
 * @{
 * - @ref Httpd::ControllerDepartments - /departments - Manipuluje s informacemi o odděleních v docházce
 * @}
 */


namespace Httpd {

/**
@brief Manipuluje s informacemi o odděleních v docházce

Vrací seznam oddělení v docházce.

Implementované metody:
- get
- put, post
- delete

Struktura jednoho záznamu: 

@code
{
"department": 23,
"abbr": "VY",
"description": "Výroba"
}
@endcode


*/
class ControllerDepartmentHasMenager : public AbstractController {
    Q_OBJECT
  public:
    ControllerDepartments(HobrasoftHttpd::HttpConnection *parent);

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
