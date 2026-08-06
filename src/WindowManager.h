#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QList>
#include <QMap>
#include <QString>
#include <QWidget>

class QSplitter;
class QTabWidget;

enum class WindowArea
{
    /*
     * 中心工作区。
     */
    Central,
    /*
     * 左侧停靠区。
     */
    LeftDock,
    /*
     * 右侧停靠区。
     */
    RightDock,
    /*
     * 底部停靠区。
     */
    BottomDock,
    /*
     * 独立浮动窗口。
     */
    Floating
};

struct WindowDescriptor
{
    /*
     * 窗口唯一标识。
     */
    QString id;
    /*
     * 窗口显示标题。
     */
    QString title;
    /*
     * 窗口目标承载区域。
     */
    WindowArea area = WindowArea::Central;
    /*
     * 窗口实际显示的 QWidget。
     */
    QWidget *widget = nullptr;
};

class WindowManager : public QWidget
{
    Q_OBJECT

public:
    /*
     * 创建窗口管理器。
     * parent 为 Qt 对象父节点，用于纳入 Qt 对象生命周期管理。
     */
    explicit WindowManager(QWidget *parent = nullptr);

    /*
     * 注册一个窗口部件。
     * descriptor 描述窗口唯一标识、标题、目标区域和实际 QWidget。
     */
    void registerWindow(const WindowDescriptor &descriptor);

    /*
     * 获取指定区域的窗口数量。
     * area 为目标窗口区域，返回该区域已经注册的窗口数量。
     */
    int windowCount(WindowArea area) const;

    /*
     * 激活指定窗口。
     * id 为注册窗口时使用的唯一标识，找到并切换到该窗口时返回 true。
     */
    bool activateWindow(const QString &id);

    /*
     * 设置窗口管理器当前主题名称。
     * themeName 用于同步浮动窗口自绘标题栏图标。
     */
    void setThemeName(const QString &themeName);

private:
    /*
     * 初始化窗口管理器界面。
     * 创建左侧、中心、右侧和底部区域布局。
     */
    void setupUi();

    /*
     * 获取区域对应的标签容器。
     * area 为目标区域，返回对应 QTabWidget，浮动区域返回空指针。
     */
    QTabWidget *tabWidgetForArea(WindowArea area) const;

    /*
     * 注册浮动窗口。
     * descriptor 描述浮动窗口内容，函数会创建独立顶层窗口承载该内容。
     */
    void registerFloatingWindow(const WindowDescriptor &descriptor);

    /*
     * 主水平分割器。
     */
    QSplitter *m_mainSplitter = nullptr;
    /*
     * 中心区域垂直分割器。
     */
    QSplitter *m_centerSplitter = nullptr;
    /*
     * 左侧区域标签容器。
     */
    QTabWidget *m_leftTabs = nullptr;
    /*
     * 中心区域标签容器。
     */
    QTabWidget *m_centralTabs = nullptr;
    /*
     * 右侧区域标签容器。
     */
    QTabWidget *m_rightTabs = nullptr;
    /*
     * 底部区域标签容器。
     */
    QTabWidget *m_bottomTabs = nullptr;
    /*
     * 各窗口区域的窗口数量统计。
     */
    QMap<WindowArea, int> m_windowCounts;
    /*
     * 已注册的非浮动窗口索引表。
     */
    QMap<QString, QPair<QTabWidget *, QWidget *>> m_registeredWindows;
    /*
     * 已注册的浮动窗口索引表。
     */
    QMap<QString, QWidget *> m_floatingWindows;
    /*
     * 当前主题名称，用于同步浮动窗口标题栏。
     */
    QString m_themeName = QStringLiteral("cyber-dark");
};

#endif // WINDOWMANAGER_H
