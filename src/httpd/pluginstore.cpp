#include "pluginstore.h"
#include "pdebug.h"
#include <QPluginLoader>
#include <QCoreApplication>
#include <QTimer>
#include <QDir>
#include <QStringList>

using namespace Httpd;

PluginStore *PluginStore::m_pluginStore = NULL;

PluginStore::PluginStore(QObject *parent) : QObject(parent) {
    m_pluginStore = this;
    QTimer::singleShot(100, this, SLOT(init()));
}


PluginStore *PluginStore::pluginStore(QObject *parent) {
    if (m_pluginStore == NULL) {
        if (parent == NULL) {
            qFatal("PluginStore::pluginStore(NULL)");
            }
        new PluginStore(parent);
        }
    return m_pluginStore;
}


void PluginStore::init() {
    QStringList plugins;
    QStringList plugindirs;
    plugindirs << QCoreApplication::applicationDirPath()+"/plugins";
    plugindirs << "/usr/lib/fotomonserver/plugins";
    for (int i=0; i<plugindirs.size(); i++) {
        QDir dir(plugindirs[i]);
        PDEBUG << "path:" << dir.absolutePath();
        QStringList files = dir.entryList(QStringList() << "lib*.so", QDir::Files);
        for (int f=0; f<files.size(); f++) {
            plugins  << dir.absolutePath()+"/"+files[f];
            }
        }

    for (int i=0; i<plugins.size(); i++) {
        const QString& filename = plugins[i];
        QPluginLoader *loader = new QPluginLoader(this);
        loader->setFileName(filename);

        if (!loader->load()) {
            PDEBUG << filename << loader->errorString();
            return;
            }
        QObject *x = loader->instance();
        if (x == NULL) { 
            PDEBUG << "Loader instance() == NULL" << filename;
            return; 
            }
        PluginInterface *plugin = qobject_cast<PluginInterface *>(x);
        if (plugin == NULL) {
            PDEBUG << "plugin == NULL" << filename;
            return;
            }
        m_plugins << plugin;

        PDEBUG << "Loaded" << plugin->description();
        }

}


QStringList PluginStore::pluginNames() const {
    QStringList names;
    for (int i=0; i<m_plugins.size(); i++) {
        names << m_plugins[i]->description();
        }
    return names;
}



