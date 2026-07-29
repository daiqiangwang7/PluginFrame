#ifndef HELLOPLUGIN_H
#define HELLOPLUGIN_H

#include "IViewPlugin.h"

class IPluginContext;
class QLabel;

class HelloPlugin : public IViewPlugin
{
    Q_OBJECT
    Q_INTERFACES(IPlugin IViewPlugin)
    Q_PLUGIN_METADATA(IID IViewPlugin_iid FILE "metadata.json")

public:
    /*
     * 创建 Hello 视图插件对象。
     * parent 为 Qt 对象父节点，可为空。
     */
    explicit HelloPlugin(QObject *parent = nullptr);
    /*
     * 析构 Hello 视图插件对象。
     * 析构时会释放插件创建的界面部件。
     */
    ~HelloPlugin() override;

    /*
     * 注入插件运行上下文。
     * context 用于访问消息总线并参与框架内部通信。
     */
    void setContext(IPluginContext *context) override;
    /*
     * 初始化 Hello 插件界面。
     * 创建包含居中文本标签的主 QWidget，成功后返回 true。
     */
    bool initialize() override;
    /*
     * 启动 Hello 插件。
     * 当前示例插件不需要额外启动逻辑。
     */
    void start() override;
    /*
     * 停止 Hello 插件。
     * 删除插件主界面部件并将内部指针置空。
     */
    void stop() override;
    /*
     * 获取插件内部名称。
     * 返回固定名称 HelloPlugin。
     */
    QString name() const override;
    /*
     * 获取插件主界面部件。
     * 返回 initialize() 创建的 QWidget 指针。
     */
    QWidget *widget() override;
    /*
     * 获取插件显示名称。
     * 返回值用于主窗口标签页标题。
     */
    QString displayName() const override;

private:
    IPluginContext *m_context = nullptr;
    QLabel *m_label = nullptr;
    QLabel *m_timeLabel = nullptr;
    QWidget *m_widget = nullptr;
};

#endif // HELLOPLUGIN_H
