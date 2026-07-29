#ifndef IPLUGIN_H
#define IPLUGIN_H

#include <QObject>
#include <QString>

class IPluginContext;

class IPlugin : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;
    /*
     * 析构插件接口对象。
     * 默认实现不做额外资源释放，具体插件可在派生类中完成清理。
     */
    ~IPlugin() override = default;

    /*
     * 注入插件运行上下文。
     * context 由宿主创建，用于向插件提供消息总线等共享能力。
     */
    virtual void setContext(IPluginContext *context) = 0;
    /*
     * 初始化插件运行所需的资源。
     * 返回 true 表示初始化成功，返回 false 表示插件不可启动。
     */
    virtual bool initialize() = 0;
    /*
     * 启动插件。
     * 该函数在 initialize() 成功后由插件管理器调用。
     */
    virtual void start() = 0;
    /*
     * 停止插件并释放运行期资源。
     * 主程序退出或插件卸载前由插件管理器调用。
     */
    virtual void stop() = 0;
    /*
     * 获取插件的内部名称。
     * 返回值用于日志、调试和插件管理标识。
     */
    virtual QString name() const = 0;
};

#define IPlugin_iid "com.myframework.IPlugin/1.0"
Q_DECLARE_INTERFACE(IPlugin, IPlugin_iid)

#endif // IPLUGIN_H
