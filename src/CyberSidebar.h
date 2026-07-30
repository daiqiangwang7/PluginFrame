#ifndef CYBERSIDEBAR_H
#define CYBERSIDEBAR_H

#include <QWidget>

class QPushButton;
class QVBoxLayout;

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

signals:
    /*
     * 请求显示指定页面。
     * pageId 为侧边栏导航项对应的页面标识。
     */
    void pageRequested(const QString &pageId);

private:
    /*
     * 初始化抽屉界面。
     * 创建右侧贴边把手、导航入口和底部占位空间。
     */
    void setupUi();

    /*
     * 添加一个导航按钮。
     * text 为按钮显示文本，pageId 为点击后发出的页面标识。
     */
    void addNavigationButton(const QString &text, const QString &pageId);

    /*
     * 刷新抽屉内容区可见状态。
     * 展开时显示导航列表，收起时仅保留贴边把手。
     */
    void updateExpandedState();

    QWidget *m_contentWidget = nullptr;
    QPushButton *m_handleButton = nullptr;
    QVBoxLayout *m_contentLayout = nullptr;
    bool m_expanded = false;
};

#endif // CYBERSIDEBAR_H
