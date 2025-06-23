#ifndef _Httpd_ControllerStatusOverview_H_
#define _Httpd_ControllerStatusOverview_H_

#include "abstractcontroller.h"

namespace Httpd {

class ControllerStatusOverview : public AbstractController {
    Q_OBJECT
public:
    ControllerStatusOverview(HobrasoftHttpd::HttpConnection *parent);

protected:
    void serviceIdGet(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) Q_DECL_OVERRIDE;
};

}

#endif
