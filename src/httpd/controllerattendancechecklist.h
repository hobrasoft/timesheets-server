#ifndef _Httpd_ControllerAttendanceChecklist_H_
#define _Httpd_ControllerAttendanceChecklist_H_

#include <QObject>
#include "abstractcontroller.h"

namespace Httpd {

class ControllerAttendanceChecklist : public AbstractController {
    Q_OBJECT
  public:
    ControllerAttendanceChecklist(HobrasoftHttpd::HttpConnection *parent);

  protected:
    void serviceList(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) Q_DECL_OVERRIDE;
};

}

#endif
