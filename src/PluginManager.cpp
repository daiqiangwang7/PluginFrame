#include "PluginManager.h"

#include "MessageBus.h"
#include "PluginContext.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QLibrary>
#include <QPluginLoader>

PluginManager::PluginManager(QObject *parent)
    : QObject(parent)
    , m_messageBus(new MessageBus(this))
    , m_context(new PluginContext(m_messageBus, this))
{
}

PluginManager::~PluginManager()
{
    for (PluginRecord &record : m_records) {
        stopPlugin(record);
    }
}

void PluginManager::loadPlugins(const QString &path)
{
    QDir pluginsDir(path);
    if (!pluginsDir.exists()) {
        qWarning() << "Plugin directory does not exist:" << path;
        return;
    }

    const QFileInfoList entries = pluginsDir.entryInfoList(QDir::Files);
    for (const QFileInfo &entry : entries) {
        const QString filePath = entry.absoluteFilePath();
        if (!QLibrary::isLibrary(filePath)) {
            continue;
        }

        QPluginLoader *loader = new QPluginLoader(filePath, this);
        QObject *instance = loader->instance();
        if (!instance) {
            qWarning() << "Failed to load plugin:" << filePath << loader->errorString();
            PluginRecord record;
            record.loader = loader;
            record.filePath = filePath;
            record.errorString = loader->errorString();
            record.state = PluginState::Failed;
            m_records.append(record);
            loader->deleteLater();
            continue;
        }

        IPlugin *plugin = qobject_cast<IPlugin *>(instance);
        if (!plugin) {
            qWarning() << "Loaded library is not an IPlugin:" << filePath;
            PluginRecord record;
            record.loader = loader;
            record.filePath = filePath;
            record.errorString = QStringLiteral("Loaded library is not an IPlugin");
            record.state = PluginState::Failed;
            m_records.append(record);
            loader->unload();
            loader->deleteLater();
            continue;
        }

        PluginRecord record;
        record.loader = loader;
        record.plugin = plugin;
        record.filePath = filePath;
        record.name = plugin->name();
        record.state = PluginState::Loaded;

        plugin->setContext(m_context);
        if (!plugin->initialize()) {
            qWarning() << "Plugin initialization failed:" << plugin->name();
            record.errorString = QStringLiteral("Plugin initialization failed");
            record.state = PluginState::Failed;
            m_records.append(record);
            loader->unload();
            loader->deleteLater();
            continue;
        }

        record.state = PluginState::Initialized;
        plugin->start();
        record.state = PluginState::Started;
        m_records.append(record);
    }
}

MessageBus *PluginManager::messageBus() const
{
    return m_messageBus;
}

QList<PluginRecord> PluginManager::pluginRecords() const
{
    return m_records;
}

void PluginManager::addPluginForTest(IPlugin *plugin)
{
    if (plugin) {
        plugin->setContext(m_context);

        PluginRecord record;
        record.plugin = plugin;
        record.name = plugin->name();
        record.state = PluginState::Loaded;

        if (plugin->initialize()) {
            record.state = PluginState::Initialized;
            plugin->start();
            record.state = PluginState::Started;
        } else {
            record.errorString = QStringLiteral("Plugin initialization failed");
            record.state = PluginState::Failed;
        }

        m_records.append(record);
    }
}

void PluginManager::stopPlugin(PluginRecord &record)
{
    if (record.state != PluginState::Started || !record.plugin) {
        return;
    }

    record.plugin->stop();
    record.state = PluginState::Stopped;
    if (record.loader) {
        record.loader->unload();
    }
}
