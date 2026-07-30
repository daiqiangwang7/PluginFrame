#ifndef CYBERSIDEBAR_H
#define CYBERSIDEBAR_H

#include <QWidget>

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
     * 当前隐藏时显示，当前显示时隐藏。
     */
    void toggleDrawer();

    /*
     * 更新抽屉几何位置。
     * 根据父窗口尺寸将抽屉停靠到右侧。
     */
    void updateDrawerGeometry();

signals:
    /*
     * 请求显示指定页面。
     * pageId 为侧边栏导航项对应的页面标识。
     */
    void pageRequested(const QString &pageId);

private:
    /*
     * 初始化抽屉界面。
     * 创建固定导航入口和底部占位空间。
     */
    void setupUi();

    /*
     * 添加一个导航按钮。
     * text 为按钮显示文本，pageId 为点击后发出的页面标识。
     */
    void addNavigationButton(const QString &text, const QString &pageId);

    QVBoxLayout *m_layout = nullptr;
};

#endif // CYBERSIDEBAR_H
