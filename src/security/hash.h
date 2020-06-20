#ifndef _HASH_H_
#define _HASH_H_

#include <QString>

namespace Security {
/**
 * @brief Implementace hash algoritmu sdbm, výstup je převedený na text
 *
 * @returns řetězec ve tvaru "2OQ10GP"
 */
QString hash(const QString&);
}

#endif
