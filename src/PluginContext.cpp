#include "PluginContext.h"

#include "MessageBus.h"

PluginContext::PluginContext(MessageBus *messageBus, QObject *parent)
    : IPluginContext(parent)
    , m_messageBus(messageBus)
{
}

MessageBus *PluginContext::messageBus() const
{
    return m_messageBus;
}
