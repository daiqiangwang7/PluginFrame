#ifndef CYBERSIDEBAR_H
#define CYBERSIDEBAR_H

#include <QList>
#include <QString>
#include <QWidget>

class QPushButton;
class QGridLayout;
class QLabel;
class QToolButton;
class QVBoxLayout;

/*
 * 描述抽屉中的一个入口。
 * 入口可指向内置页面，也可指向插件浮动窗口。
 */
struct DrawerEntry
{
    QString id;
    QString text;
    QString iconName;
    bool windowEntry = false;
};

class CyberSidebar : public QWidget
{
    Q_OBJECT

public:
    /*
     * 创建赛博朋克功能抽屉。
     * parent 为 Qt 对象父节点，用于纳入 Qt 对象生命周期管理。
     */
    explicit CyberSidebar(QWidget *parent = nullptr);

    /*
     * 切换抽屉显示状态。
     * 当前收起时展开导航列表，当前展开时收起为右侧贴边把手。
     */
    void toggleDrawer();

    /*
     * 更新抽屉几何位置。
     * 根据父窗口尺寸将抽屉停靠到右侧。
     */
    void updateDrawerGeometry();

    /*
     * 返回抽屉是否处于展开状态。
     * 用于外部判断当前导航列表是否完整显示。
     */
    bool isExpanded() const;

    /*
     * 添加插件浮动窗口入口。
     * windowId 为窗口管理器中的窗口标识，text 为抽屉显示名称。
     */
    void addWindowEntry(const QString &windowId, const QString &text);

    /*
     * 设置当前主题名称。
     * themeName 用于选择深色或浅色抽屉图标。
     */
    void setThemeName(const QString &themeName);

signals:
    /*
     * 请求显示指定页面。
     * pageId 为侧边栏导航项对应的页面标识。
     */
    void pageRequested(const QString &pageId);

    /*
     * 请求显示指定浮动窗口。
     * windowId 为窗口管理器中注册的窗口标识。
     */
    void windowRequested(const QString &windowId);

private:
    /*
     * 初始化抽屉界面。
     * 创建右侧贴边把手、导航入口和底部占位空间。
     */
    void setupUi();

    /*
     * 添加一个内置页面入口。
     * text 为按钮显示文本，pageId 为点击后发出的页面标识，iconName 为图标名称。
     */
    void addPageEntry(const QString &text, const QString &pageId, const QString &iconName);

    /*
     * 刷新抽屉内容区可见状态。
     * 展开时显示导航列表，收起时仅保留贴边把手。
     */
    void updateExpandedState();

    /*
     * 刷新当前页入口按钮。
     * 根据当前页码将最多十个入口排列为两列五行。
     */
    void rebuildEntryButtons();

    /*
     * 切换到上一页入口。
     * 当前已经是第一页时保持不变。
     */
    void previousPage();

    /*
     * 切换到下一页入口。
     * 当前已经是最后一页时保持不变。
     */
    void nextPage();

    /*
     * 获取当前主题的抽屉图标路径。
     * iconName 为图标文件基础名称，返回 qrc 图标路径。
     */
    QString drawerIconPath(const QString &iconName) const;

    QWidget *m_contentWidget = nullptr;
    QPushButton *m_handleButton = nullptr;
    QWidget *m_entriesWidget = nullptr;
    QGridLayout *m_entriesLayout = nullptr;
    QVBoxLayout *m_contentLayout = nullptr;
    QWidget *m_pagerWidget = nullptr;
    QPushButton *m_previousButton = nullptr;
    QPushButton *m_nextButton = nullptr;
    QLabel *m_pageLabel = nullptr;
    QList<DrawerEntry> m_entries;
    QList<QToolButton *> m_entryButtons;
    QString m_themeName = QStringLiteral("cyber-dark");
    int m_currentPage = 0;
    bool m_expanded = false;
};

#endif // CYBERSIDEBAR_H
