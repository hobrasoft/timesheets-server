/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _Database_H_
#define _Database_H_

#include <QObject>
#include <QVariant>
#include "dbt.h"

#define DB Db::Database::instance(this)

namespace Db {

struct PluginDescription {
    QString     name;
    QString     description;
    PluginDescription(const QString& n, const QString& d) { name=n; description=d; }
};

namespace Plugins {
class DatabasePlugin;
}

/**
 * @brief
 */
class Database : public QObject {
    Q_OBJECT
  public:
   ~Database();
    static Database *instance(QObject *parent = nullptr);
    static Database *create(QObject *parent);
    bool isOpen() const { return m_isOpen; }
    QString     connectionName() const;

    QList<Dbt::Users>                       authenticate(const QString& user, const QString& password);

    void        begin();
    void        commit();

  signals:
    void    opened(bool);

  public slots:
    bool    open();
    void    close();

  private slots:

  private:
    static Database *m_instance;
    Database(QObject *parent);
    bool m_isOpen;

    Db::Plugins::DatabasePlugin *m_dbplugin;

};

}

#endif

