#ifndef PLUGINCONTEXT_H
#define PLUGINCONTEXT_H

#include "IPluginContext.h"

class CapabilityRegistry;
class LogService;
class MessageBus;
class PluginSettings;

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

    /*
     * 获取插件配置服务。
     * 返回构造时创建的共享 PluginSettings 指针。
     */
    PluginSettings *pluginSettings() const override;

    /*
     * 获取框架日志服务。
     * 返回构造时创建的共享 LogService 指针。
     */
    LogService *logService() const override;

private:
    CapabilityRegistry *m_capabilityRegistry = nullptr;
    LogService *m_logService = nullptr;
    MessageBus *m_messageBus = nullptr;
    PluginSettings *m_pluginSettings = nullptr;
};

#endif // PLUGINCONTEXT_H
