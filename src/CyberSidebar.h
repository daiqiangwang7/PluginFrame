#ifndef CYBERSIDEBAR_H
#define CYBERSIDEBAR_H

#include <QWidget>

class QVBoxLayout;

class CyberSidebar : public QWidget
{
    Q_OBJECT

public:
    /*
     * 创建赛博朋克侧边栏。
     * parent 为 Qt 对象父节点，用于纳入 Qt 对象生命周期管理。
     */
    explicit CyberSidebar(QWidget *parent = nullptr);

signals:
    /*
     * 请求显示指定页面。
     * pageId 为侧边栏导航项对应的页面标识。
     */
    void pageRequested(const QString &pageId);

private:
    /*
     * 初始化侧边栏界面。
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
