#include "PluginManager.h"

#include "CapabilityRegistry.h"
#include "MessageBus.h"
#include "PluginContext.h"
#include "PluginSettings.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QJsonObject>
#include <QLibrary>
#include <QPluginLoader>
#include <QSet>

#include <algorithm>

namespace {

int pluginTypePriority(const PluginMetadata &metadata)
{
    return metadata.type == QStringLiteral("service") ? 0 : 1;
}

} // namespace

QString pluginStateToString(PluginState state)
{
    switch (state) {
    case PluginState::Loaded:
        return QStringLiteral("已加载");
    case PluginState::Initialized:
        return QStringLiteral("已初始化");
    case PluginState::Started:
        return QStringLiteral("运行中");
    case PluginState::Stopped:
        return QStringLiteral("已停止");
    case PluginState::Disabled:
        return QStringLiteral("已禁用");
    case PluginState::Failed:
        return QStringLiteral("失败");
    }

    return QStringLiteral("未知");
}

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

    if (pluginSettings()) {
        const QString settingsPath = QFileInfo(pluginsDir.absolutePath()).dir().absoluteFilePath(QStringLiteral("config/plugins"));
        pluginSettings()->setBasePath(settingsPath);
    }

    const QFileInfoList entries = pluginsDir.entryInfoList(QDir::Files, QDir::Name);
    for (const QFileInfo &entry : entries) {
        const QString filePath = entry.absoluteFilePath();
        if (!QLibrary::isLibrary(filePath)) {
            continue;
        }

        QPluginLoader *loader = new QPluginLoader(filePath, this);
        const QJsonObject rootMetadata = loader->metaData();
        const QJsonObject jsonMetadata = rootMetadata.value(QStringLiteral("MetaData")).toObject();
        QString metadataError;
        const PluginMetadata metadata = PluginMetadata::fromJsonObject(jsonMetadata, &metadataError);
        if (!metadata.isValid()) {
            qWarning() << "Invalid plugin metadata:" << filePath << metadataError;
            PluginRecord record;
            record.loader = loader;
            record.filePath = filePath;
            record.errorString = metadataError;
            record.state = PluginState::Failed;
            m_records.append(record);
            loader->deleteLater();
            continue;
        }

        const bool enabledByConfig = m_enabledOverrides.value(metadata.id, true);
        if (!metadata.enabled || !enabledByConfig) {
            PluginRecord record;
            record.loader = loader;
            record.filePath = filePath;
            record.name = metadata.name;
            record.metadata = metadata;
            record.state = PluginState::Disabled;
            m_records.append(record);
            loader->deleteLater();
            continue;
        }

        PluginRecord record;
        record.loader = loader;
        record.filePath = filePath;
        record.name = metadata.name;
        record.metadata = metadata;
        record.state = PluginState::Loaded;
        m_records.append(record);
    }

    startScannedPlugins();
}

MessageBus *PluginManager::messageBus() const
{
    return m_messageBus;
}

CapabilityRegistry *PluginManager::capabilityRegistry() const
{
    return m_context ? m_context->capabilityRegistry() : nullptr;
}

PluginSettings *PluginManager::pluginSettings() const
{
    return m_context ? m_context->pluginSettings() : nullptr;
}

void PluginManager::setPluginEnabled(const QString &pluginId, bool enabled)
{
    m_enabledOverrides.insert(pluginId, enabled);
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
        record.metadata.id = plugin->name();
        record.metadata.name = plugin->name();
        record.metadata.displayName = plugin->name();
        record.metadata.version = QStringLiteral("test");
        record.metadata.type = QStringLiteral("service");
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

    if (capabilityRegistry()) {
        capabilityRegistry()->unregisterPluginCapabilities(record.metadata.id);
    }
    record.plugin->stop();
    record.state = PluginState::Stopped;
    if (record.loader) {
        record.loader->unload();
    }
}

void PluginManager::startScannedPlugins()
{
    QSet<int> pendingIndexes;
    QSet<QString> knownPluginIds;
    QSet<QString> startedPluginIds;

    for (int index = 0; index < m_records.size(); ++index) {
        const PluginRecord &record = m_records.at(index);
        if (record.metadata.isValid()) {
            knownPluginIds.insert(record.metadata.id);
        }
        if (record.state == PluginState::Loaded) {
            pendingIndexes.insert(index);
        }
    }

    bool progressed = true;
    while (!pendingIndexes.isEmpty() && progressed) {
        progressed = false;
        QList<int> readyIndexes;

        for (int index : pendingIndexes) {
            const PluginRecord &record = m_records.at(index);
            bool missingKnownDependency = false;
            for (const QString &dependencyId : record.metadata.dependencies) {
                if (!knownPluginIds.contains(dependencyId)) {
                    missingKnownDependency = true;
                    break;
                }
            }

            if (!missingKnownDependency && dependenciesSatisfied(record, startedPluginIds)) {
                readyIndexes.append(index);
            }
        }

        std::sort(readyIndexes.begin(), readyIndexes.end(), [this](int left, int right) {
            const PluginMetadata &leftMetadata = m_records.at(left).metadata;
            const PluginMetadata &rightMetadata = m_records.at(right).metadata;
            const int leftPriority = pluginTypePriority(leftMetadata);
            const int rightPriority = pluginTypePriority(rightMetadata);
            if (leftPriority != rightPriority) {
                return leftPriority < rightPriority;
            }
            return leftMetadata.id < rightMetadata.id;
        });

        for (int index : readyIndexes) {
            if (startPlugin(index)) {
                startedPluginIds.insert(m_records.at(index).metadata.id);
            }
            pendingIndexes.remove(index);
            progressed = true;
        }
    }

    if (!pendingIndexes.isEmpty()) {
        failUnresolvedDependencies(pendingIndexes);
    }
}

bool PluginManager::startPlugin(int index)
{
    if (index < 0 || index >= m_records.size()) {
        return false;
    }

    PluginRecord &record = m_records[index];
    QPluginLoader *loader = record.loader;
    if (!loader) {
        record.errorString = QStringLiteral("Plugin loader is null");
        record.state = PluginState::Failed;
        return false;
    }

        QObject *instance = loader->instance();
        if (!instance) {
            qWarning() << "Failed to load plugin:" << record.filePath << loader->errorString();
            record.errorString = loader->errorString();
            record.state = PluginState::Failed;
            loader->deleteLater();
            return false;
        }

        IPlugin *plugin = qobject_cast<IPlugin *>(instance);
        if (!plugin) {
            qWarning() << "Loaded library is not an IPlugin:" << record.filePath;
            record.errorString = QStringLiteral("Loaded library is not an IPlugin");
            record.state = PluginState::Failed;
            loader->unload();
            loader->deleteLater();
            return false;
        }

        record.plugin = plugin;
        record.state = PluginState::Loaded;

        plugin->setContext(m_context);
        if (!plugin->initialize()) {
            qWarning() << "Plugin initialization failed:" << plugin->name();
            record.errorString = QStringLiteral("Plugin initialization failed");
            record.state = PluginState::Failed;
            loader->unload();
            loader->deleteLater();
            return false;
        }

        record.state = PluginState::Initialized;
        plugin->start();
        record.state = PluginState::Started;
        return true;
}

bool PluginManager::dependenciesSatisfied(const PluginRecord &record, const QSet<QString> &startedPluginIds) const
{
    for (const QString &dependencyId : record.metadata.dependencies) {
        if (!startedPluginIds.contains(dependencyId)) {
            return false;
        }
    }
    return true;
}

void PluginManager::failUnresolvedDependencies(const QSet<int> &pendingIndexes)
{
    QSet<QString> availableStartedIds;
    QSet<QString> knownIds;
    for (const PluginRecord &record : m_records) {
        if (record.metadata.isValid()) {
            knownIds.insert(record.metadata.id);
        }
        if (record.state == PluginState::Started) {
            availableStartedIds.insert(record.metadata.id);
        }
    }

    for (int index : pendingIndexes) {
        PluginRecord &record = m_records[index];
        QStringList missingDependencies;
        for (const QString &dependencyId : record.metadata.dependencies) {
            if (!knownIds.contains(dependencyId) || !availableStartedIds.contains(dependencyId)) {
                missingDependencies.append(dependencyId);
            }
        }

        record.errorString = missingDependencies.isEmpty()
                ? QStringLiteral("Plugin dependency cycle detected")
                : QStringLiteral("Plugin dependencies not satisfied: %1").arg(missingDependencies.join(QStringLiteral(", ")));
        record.state = PluginState::Failed;
        if (record.loader) {
            record.loader->deleteLater();
        }
        qWarning() << "Plugin dependencies not satisfied:" << record.metadata.id << record.errorString;
    }
}
