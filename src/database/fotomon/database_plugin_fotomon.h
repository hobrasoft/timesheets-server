/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _DatabasePluginFotomon_H_
#define _DatabasePluginFotomon_H_

#include "database_plugin.h"
#include "msqlquery.h"

namespace Db {
namespace Plugins {


/**
 * @brief
 */
class DatabasePluginFotomon : public Db::Plugins::DatabasePlugin {
    Q_OBJECT
  public:
    DatabasePluginFotomon(QObject *);
   ~DatabasePluginFotomon();

    void setDatabaseName(const QString& x) override { m_databasename = x; }
    void setHostname(const QString& x) override { m_hostname = x; }
    void setPort(int x) override { m_port = x; }
    void setUserName(const QString& x) override { m_username = x; }
    void setPassword(const QString& x) override { m_password = x; }
  
    QList<Dbt::Users>                       authenticate(const QString& login, const QString& password) override;
    QList<Dbt::Categories>                  categories() override;
    QList<Dbt::StatusOrder>                 statusOrder() override;
    QList<Dbt::Statuses>                    statuses() override;


    bool    open() override; 
    bool    close() override;
    void    begin() override;
    void    commit() override;

  private:
    void            createCategoriesTemporaryTable();
    static QString  parentCategoryKey(const QVariant& type, const QVariant& system, const QVariant& category, int parent_type);
    static QString        categoryKey(const QVariant& type, const QVariant& system, const QVariant& category, int parent_type);


    QString         m_databasename;
    QString         m_hostname;
    int             m_port;
    QString         m_username;
    QString         m_password;
    QString         m_lang;
    static int      m_user;

};

}
}

#endif

