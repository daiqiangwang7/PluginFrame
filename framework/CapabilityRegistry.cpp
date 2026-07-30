#include "CapabilityRegistry.h"

CapabilityRegistry::CapabilityRegistry(QObject *parent)
    : QObject(parent)
{
}

bool CapabilityRegistry::registerCapability(const PluginCapability &capability)
{
    if (capability.pluginId.trimmed().isEmpty()
            || capability.id.trimmed().isEmpty()
            || capability.type.trimmed().isEmpty()
            || capability.displayName.trimmed().isEmpty()) {
        return false;
    }

    for (PluginCapability &existing : m_capabilities) {
        if (existing.pluginId == capability.pluginId && existing.id == capability.id) {
            existing = capability;
            emit capabilitiesChanged();
            return true;
        }
    }

    m_capabilities.append(capability);
    emit capabilitiesChanged();
    return true;
}

void CapabilityRegistry::unregisterPluginCapabilities(const QString &pluginId)
{
    bool changed = false;
    for (int index = m_capabilities.size() - 1; index >= 0; --index) {
        if (m_capabilities.at(index).pluginId == pluginId) {
            m_capabilities.removeAt(index);
            changed = true;
        }
    }

    if (changed) {
        emit capabilitiesChanged();
    }
}

QList<PluginCapability> CapabilityRegistry::capabilities() const
{
    return m_capabilities;
}

QList<PluginCapability> CapabilityRegistry::findCapabilities(const QString &type) const
{
    QList<PluginCapability> result;
    for (const PluginCapability &capability : m_capabilities) {
        if (capability.type == type) {
            result.append(capability);
        }
    }
    return result;
}
