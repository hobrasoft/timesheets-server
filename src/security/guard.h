/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz
 */
#ifndef _Guard_H_
#define _Guard_H_

#define GUARD Security::Guard::guard()

#include "permissions.h"
#include "roles.h"
#include <QObject>
#include <QHash>
#include <QSet>


/**
 * @brief Jmenný prostor pro správu oprávnění, role, klíče a náhodná volání 
 */
namespace Security {

/**
 * @brief Definuje a poskytuje informace o přístupových oprávněních
 *////
class Guard : public QObject {
    Q_OBJECT
  public:
    /**
     * @brief Vrací ukazatel na singleton instanci třídy
     */
    static Guard *guard(QObject *parent = NULL);

    /**
     * @brief Vrací true, pokud je požadované oprávnění pro požadovanou roli přístupné
     */
    bool isGranted(Permissions::Permission, Roles::Role) const;

  private:
    Guard(QObject *);
    #ifndef DOXYGEN_SHOULD_SKIP_THIS
    static Guard *m_guard;
    QHash<Roles::Role, QSet<Permissions::Permission> >  m_grants;
    #endif

};

}

#endif
