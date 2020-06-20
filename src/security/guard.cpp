/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz
 */
#include "guard.h"

using namespace Security;

Guard *Guard::m_guard  = NULL;


Guard *Guard::guard(QObject *parent) {
    if (m_guard == NULL) {
        if (parent == NULL) {
            qFatal("Guard::guard(NULL)");
            }
        new Guard(parent);
        }
    return m_guard;
}


/**
 * @brief Konstruktor - zde se vytváří seznam jednotlivých oprávnění
 *
 * Konstruktor je privátní, volá se přes funkci guard() kvůli singletonu
 *
 * Do kontejneru m_grants se pro každé oprávnění (momentálně jen Roles::User) 
 * vloží každé oprávnění, kterým by měla daná role disponovat.
 *
 * Pro uživatele admin není třeba práva zadávat, admin má všechna práva automaticky.
 */
Guard::Guard(QObject *parent) : QObject(parent) {
    m_guard = this;
    m_grants[Roles::User]
        << Permissions::Server
        << Permissions::ServerConfigFile
        << Permissions::SystemPower
        << Permissions::Systems
        << Permissions::Translations
        ;
}


bool Guard::isGranted(Permissions::Permission permission, Roles::Role role) const {
    if (role == Roles::Admin) {
        return true;
        }
    if (m_grants.contains(role) && m_grants[role].contains(permission)) {
        return true;
        }
    return false;
}

