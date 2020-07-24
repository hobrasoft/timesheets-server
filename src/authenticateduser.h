/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _AuthenticatedUser_H_
#define _AuthenticatedUser_H_

#include <QObject>

/**
 * @brief
 */
class AuthenticatedUser : public QObject {
    Q_OBJECT
  public:
    AuthenticatedUser(QObject *parent);

    int user() const { return m_user; }
    QString name() const { return m_name; }
    QString login() const { return m_login; }
    QString lang() const { return m_lang; }


    void setAuthenticated(bool x) { m_authenticated = x; }
    bool authenticated() const { return m_authenticated; }

    bool authenticate(const QString& login, const QString& password);

  private:
    bool m_authenticated;
    int  m_user;
    QString m_name;
    QString m_login;
    QString m_lang;

};

#endif

