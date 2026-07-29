#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class PluginManager;
class QTabWidget;

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
     * 创建 QTabWidget 作为中央部件并设置窗口基础属性。
     */
    void setupUi();
    /*
     * 加载应用程序目录下的插件。
     * 视图插件会被添加到中央标签页中显示。
     */
    void loadPlugins();

    QTabWidget *m_tabs = nullptr;
    PluginManager *m_pluginManager = nullptr;
};

#endif // MAINWINDOW_H
