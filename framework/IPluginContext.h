#ifndef IPLUGINCONTEXT_H
#define IPLUGINCONTEXT_H

#include <QObject>

class MessageBus;

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
};

#endif // IPLUGINCONTEXT_H
