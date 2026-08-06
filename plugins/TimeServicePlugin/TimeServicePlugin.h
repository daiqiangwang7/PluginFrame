#ifndef TIMESERVICEPLUGIN_H
#define TIMESERVICEPLUGIN_H

#include "IServicePlugin.h"

class IPluginContext;
class QTimer;

class TimeServicePlugin : public IServicePlugin
{
    Q_OBJECT
    Q_INTERFACES(IPlugin IServicePlugin)
    Q_PLUGIN_METADATA(IID IServicePlugin_iid FILE "metadata.json")

public:
    /*
     * 创建时间服务插件对象。
     * parent 为 Qt 对象父节点，可为空。
     */
    explicit TimeServicePlugin(QObject *parent = nullptr);
    /*
     * 析构时间服务插件对象。
     * 析构时会停止后台定时器。
     */
    ~TimeServicePlugin() override;

    /*
     * 注入插件运行上下文。
     * context 用于访问消息总线并发布 time.tick 消息。
     */
    void setContext(IPluginContext *context) override;
    /*
     * 初始化时间服务插件。
     * 创建后台定时器并连接定时发布逻辑。
     */
    bool initialize() override;
    /*
     * 启动时间服务插件。
     * 启动定时器并立即发布一次 time.tick 消息。
     */
    void start() override;
    /*
     * 停止时间服务插件。
     * 停止后台定时器。
     */
    void stop() override;
    /*
     * 获取插件内部名称。
     * 返回固定名称 TimeServicePlugin。
     */
    QString name() const override;

private:
    /*
     * 发布当前时间消息。
     * 消息主题为 time.tick，payload 中包含格式化时间字符串。
     */
    void publishTick();

    /*
     * 插件运行上下文。
     */
    IPluginContext *m_context = nullptr;
    /*
     * 定时发布时间消息的后台定时器。
     */
    QTimer *m_timer = nullptr;
};

#endif // TIMESERVICEPLUGIN_H
