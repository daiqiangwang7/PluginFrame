#ifndef PLUGINCONTEXT_H
#define PLUGINCONTEXT_H

#include "IPluginContext.h"

class CapabilityRegistry;
class MessageBus;

class PluginContext : public IPluginContext
{
    Q_OBJECT

public:
    /*
     * 创建插件上下文。
     * messageBus 为宿主创建的共享消息总线，parent 为 Qt 对象父节点。
     */
    explicit PluginContext(MessageBus *messageBus, QObject *parent = nullptr);

    /*
     * 获取框架消息总线。
     * 返回构造时传入的共享 MessageBus 指针。
     */
    MessageBus *messageBus() const override;

    /*
     * 获取插件能力注册中心。
     * 返回构造时创建的共享 CapabilityRegistry 指针。
     */
    CapabilityRegistry *capabilityRegistry() const override;

private:
    CapabilityRegistry *m_capabilityRegistry = nullptr;
    MessageBus *m_messageBus = nullptr;
};

#endif // PLUGINCONTEXT_H
