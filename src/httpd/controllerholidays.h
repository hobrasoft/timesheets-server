/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerHolidays_H_
#define _Httpd_ControllerHolidays_H_

#include <QObject>
#include "abstractcontroller.h"

/**
 * @addtogroup apiurl
 * @{ 
 * - @ref Httpd::ControllerHolidays - /holidays - Manipuluje se seznamem svátků
 * @}
 */

namespace Httpd {

class ControllerHolidays : public AbstractController {
    Q_OBJECT
  public:
    ControllerHolidays(HobrasoftHttpd::HttpConnection *parent);

  protected:
    void service (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) Q_DECL_OVERRIDE;
    void serviceList     (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) Q_DECL_OVERRIDE;
    void serviceIdGet    (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) Q_DECL_OVERRIDE;
    void serviceIdPut    (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) Q_DECL_OVERRIDE;
    void serviceIdPost   (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) Q_DECL_OVERRIDE;
    void serviceIdDelete (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) Q_DECL_OVERRIDE;
    void serviceCopy     (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, int fromYear, int toYear);
};

}

#endif
