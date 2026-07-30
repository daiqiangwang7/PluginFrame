#include "PluginContext.h"

#include "CapabilityRegistry.h"
#include "LogService.h"
#include "MessageBus.h"
#include "PluginSettings.h"

PluginContext::PluginContext(MessageBus *messageBus, QObject *parent)
    : IPluginContext(parent)
    , m_capabilityRegistry(new CapabilityRegistry(this))
    , m_logService(new LogService(this))
    , m_messageBus(messageBus)
    , m_pluginSettings(new PluginSettings(QString(), this))
{
}

MessageBus *PluginContext::messageBus() const
{
    return m_messageBus;
}

CapabilityRegistry *PluginContext::capabilityRegistry() const
{
    return m_capabilityRegistry;
}

PluginSettings *PluginContext::pluginSettings() const
{
    return m_pluginSettings;
}

LogService *PluginContext::logService() const
{
    return m_logService;
}
