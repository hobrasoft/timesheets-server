/**
 * @file
 */

#ifndef _Httpd_ControllerWorkCalendar_H_
#define _Httpd_ControllerWorkCalendar_H_

#include <QObject>
#include "abstractcontroller.h"

namespace Httpd {

class ControllerWorkCalendar : public AbstractController {
    Q_OBJECT
  public:
    ControllerWorkCalendar(HobrasoftHttpd::HttpConnection *parent);

  protected:
    void serviceList     (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) Q_DECL_OVERRIDE;
    void serviceIdGet    (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) Q_DECL_OVERRIDE;
    void serviceIdPut    (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) Q_DECL_OVERRIDE;
    void serviceIdPost   (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) Q_DECL_OVERRIDE;
    void serviceIdDelete (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) Q_DECL_OVERRIDE;
};

}

#endif
