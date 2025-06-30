#ifndef _Httpd_ControllerDepartmentHasManager_H_
#define _Httpd_ControllerDepartmentHasManager_H_

#include "abstractcontroller.h"

namespace Httpd {

class ControllerDepartmentHasManager : public AbstractController {
    Q_OBJECT
public:
    ControllerDepartmentHasManager(HobrasoftHttpd::HttpConnection *parent);

protected:
    void service(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) Q_DECL_OVERRIDE;
    void serviceGet(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, int department, int user);
    void servicePut(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, int department, const QVariantMap &data);
    void serviceDelete(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, int department, int user);
};

}

#endif
