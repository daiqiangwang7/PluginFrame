#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class PluginManager;
class ThemeManager;
class TitleBar;
class QTabWidget;
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

private:
    /*
     * 初始化主窗口界面。
     * 创建自绘标题栏和内容区并设置窗口基础属性。
     */
    void setupUi();
    /*
     * 加载应用程序目录下的插件。
     * 视图插件会被添加到中央标签页中显示。
     */
    void loadPlugins();

    /*
     * 切换当前应用皮肤。
     * 在深色和浅色赛博朋克主题之间切换。
     */
    void toggleTheme();

    QWidget *m_shellRoot = nullptr;
    TitleBar *m_titleBar = nullptr;
    QTabWidget *m_tabs = nullptr;
    PluginManager *m_pluginManager = nullptr;
    ThemeManager *m_themeManager = nullptr;
};

#endif // MAINWINDOW_H
