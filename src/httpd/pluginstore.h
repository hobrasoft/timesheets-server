#ifndef _PluginStore_H_
#define _PluginStore_H_

#include <QObject>
#include <QString>
#include <QStringList>
#include "plugininterface.h"

/** @addtogroup Makra
 * @{
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define PLUGINSTORE PluginStore::pluginStore()
#endif
/** @} */

namespace Httpd {

/**
 * @brief Seznam načtených pluginů
 */
class PluginStore : public QObject {
    Q_OBJECT
  public:
    static PluginStore *pluginStore(QObject *parent = NULL);

    QList<PluginInterface *> plugins() const { return m_plugins; }
    QStringList pluginNames() const;

  private slots:
    void init();

  private:
    PluginStore(QObject *);
    static PluginStore *m_pluginStore;

    QList<PluginInterface *>    m_plugins;
};

}

#endif
