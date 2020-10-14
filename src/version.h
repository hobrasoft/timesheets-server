/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _VERSION_H_
#define _VERSION_H_

#define VERSION "1.0.1"


#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define SETTINGS_ORGANIZATION_NAME    "hobrasoft.cz"
#define SETTINGS_ORGANIZATION_DOMAIN  "hobrasoft.cz"
#define SETTINGS_APPLICATION          "timesheets"
#endif

inline QString qtVersion() {
    int x,a,b,c;
    x = QT_VERSION;
    a = x%256;
    x = x/256;
    b = x%256;
    x = x/256;
    c = x%256;
    return QString("%1.%2.%3").arg(c).arg(b).arg(a);
}


#endif

