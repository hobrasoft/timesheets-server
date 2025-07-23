#ifndef _Httpd_ControllerAttendancePresent_H_
#define _Httpd_ControllerAttendancePresent_H_

#include <QObject>
#include "abstractcontroller.h"

namespace Httpd {

class ControllerAttendancePresent : public AbstractController {
    Q_OBJECT
  public:
    ControllerAttendancePresent(HobrasoftHttpd::HttpConnection *parent);

  protected:
    void serviceList(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) Q_DECL_OVERRIDE;
};

}

#endif
