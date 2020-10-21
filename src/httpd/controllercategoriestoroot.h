/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerCategoriesToRoot_H_
#define _Httpd_ControllerCategoriesToRoot_H_

#include <QObject>
#include "abstractcontroller.h"

/**
 * @addtogroup apiurl
 * @{
 * - @ref Httpd::ControllerCategoriesToRoot - /categoriestoroot - Manipulace s kategoriemi
 * @}
 */
namespace Httpd {

/**
@brief Vrací seznam kategorií od zadané kategorie až k rootu

Implementuje metody:
- get

@see ControllerCategoriesToRoot

*/
class ControllerCategoriesToRoot : public AbstractController {
    Q_OBJECT
  public:
    ControllerCategoriesToRoot(HobrasoftHttpd::HttpConnection *parent);

  protected:
    void serviceIdGet (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) Q_DECL_OVERRIDE;

  private slots:

  private:

};

}

#endif
