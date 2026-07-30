#ifndef IPLUGINCONTEXT_H
#define IPLUGINCONTEXT_H

#include <QObject>

class CapabilityRegistry;
class MessageBus;
class PluginSettings;

class IPluginContext : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;
    /*
     * 析构插件上下文接口对象。
     * 默认实现不释放共享服务，具体生命周期由宿主侧管理。
     */
    ~IPluginContext() override = default;

    /*
     * 获取框架消息总线。
     * 返回值用于插件与宿主、插件与插件之间发布和订阅消息。
     */
    virtual MessageBus *messageBus() const = 0;

    /*
     * 获取插件能力注册中心。
     * 返回值用于插件向宿主注册窗口、命令、服务和设置页等能力。
     */
    virtual CapabilityRegistry *capabilityRegistry() const = 0;

    /*
     * 获取插件配置服务。
     * 返回值用于插件读取和保存自己的私有配置。
     */
    virtual PluginSettings *pluginSettings() const = 0;
};

#endif // IPLUGINCONTEXT_H
