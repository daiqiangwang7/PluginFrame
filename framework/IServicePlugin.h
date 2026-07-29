#ifndef ISERVICEPLUGIN_H
#define ISERVICEPLUGIN_H

#include "IPlugin.h"

class IServicePlugin : public IPlugin
{
    Q_OBJECT

public:
    using IPlugin::IPlugin;
    /*
     * 析构服务插件接口对象。
     * 服务插件不提供界面，具体后台资源由派生类释放。
     */
    ~IServicePlugin() override = default;
};

#define IServicePlugin_iid "com.myframework.IServicePlugin/1.0"
Q_DECLARE_INTERFACE(IServicePlugin, IServicePlugin_iid)

#endif // ISERVICEPLUGIN_H
