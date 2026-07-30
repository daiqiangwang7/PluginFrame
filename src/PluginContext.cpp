#include "PluginContext.h"

#include "CapabilityRegistry.h"
#include "MessageBus.h"

PluginContext::PluginContext(MessageBus *messageBus, QObject *parent)
    : IPluginContext(parent)
    , m_capabilityRegistry(new CapabilityRegistry(this))
    , m_messageBus(messageBus)
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
