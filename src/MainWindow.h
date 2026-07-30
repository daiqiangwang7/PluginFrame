#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

struct AppConfig;
class CyberSidebar;
class PluginManager;
class StatusBarWidget;
class ThemeManager;
class TitleBar;
class WindowManager;
class QWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*
     * 创建主窗口。
     * 构造过程中会初始化标签页界面并加载插件目录中的视图插件。
     */
    explicit MainWindow(QWidget *parent = nullptr);
    /*
     * 析构主窗口。
     * 子对象由 Qt 父子对象机制自动释放。
     */
    ~MainWindow() override = default;

protected:
    /*
     * 处理窗口尺寸变化事件。
     * 用于同步右侧功能抽屉的位置和高度。
     */
    void resizeEvent(QResizeEvent *event) override;

private:
    /*
     * 初始化主窗口界面。
     * 创建自绘标题栏和内容区并设置窗口基础属性。
     */
    void setupUi();
    /*
     * 加载应用程序目录下的插件。
     * 视图插件会被注册到窗口管理器的中心区域。
     */
    void loadPlugins();

    /*
     * 切换当前应用皮肤。
     * 在深色和浅色赛博朋克主题之间切换。
     */
    void toggleTheme();

    /*
     * 加载应用 XML 配置。
     * 返回解析后的配置，配置不存在或解析失败时返回默认配置。
     */
    AppConfig loadAppConfig() const;

    /*
     * 刷新底部状态栏。
     * 使用当前主题和插件记录更新工作台状态摘要。
     */
    void updateStatusBar();

    QWidget *m_shellRoot = nullptr;
    CyberSidebar *m_sidebar = nullptr;
    StatusBarWidget *m_statusBar = nullptr;
    TitleBar *m_titleBar = nullptr;
    WindowManager *m_windowManager = nullptr;
    PluginManager *m_pluginManager = nullptr;
    ThemeManager *m_themeManager = nullptr;
};

#endif // MAINWINDOW_H
