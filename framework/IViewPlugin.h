#ifndef IVIEWPLUGIN_H
#define IVIEWPLUGIN_H

#include "IPlugin.h"

#include <QString>
#include <QWidget>

class IViewPlugin : public IPlugin
{
    Q_OBJECT

public:
    using IPlugin::IPlugin;
    /*
     * 析构视图插件接口对象。
     * 默认实现不做额外资源释放，具体插件负责清理自己的界面资源。
     */
    ~IViewPlugin() override = default;

    /*
     * 获取插件提供的主界面部件。
     * 返回值由主窗口添加到标签页中显示。
     */
    virtual QWidget *widget() = 0;
    /*
     * 获取视图插件的显示名称。
     * 返回值作为主窗口标签页标题。
     */
    virtual QString displayName() const = 0;
};

#define IViewPlugin_iid "com.myframework.IViewPlugin/1.0"
Q_DECLARE_INTERFACE(IViewPlugin, IViewPlugin_iid)

#endif // IVIEWPLUGIN_H
