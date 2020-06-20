#include "hash.h"

using namespace Security;

QString hash(const QString& x) {
    int32_t h = 0;
    for (int i=0; i<x.length(); i++) {
        h = (h<<6) + (h<<16) - h + x[i].unicode();
        }
    long long int hx = h;
    hx = hx + 4294967296LL;
    return QString::number(hx,36).toUpper();
}


