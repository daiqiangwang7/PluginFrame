#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>

class QLabel;
class QPushButton;

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    /*
     * 创建自绘标题栏。
     * parent 为所属顶层窗口，标题栏会向该窗口发送最小化、最大化和关闭操作。
     */
    explicit TitleBar(QWidget *parent = nullptr);

    /*
     * 设置标题栏显示文本。
     * title 为窗口标题，会显示在标题栏左侧。
     */
    void setTitle(const QString &title);

signals:
    /*
     * 请求切换应用主题。
     * 用户点击主题按钮时发出该信号。
     */
    void themeToggleRequested();

protected:
    /*
     * 处理鼠标按下事件。
     * 用于记录拖动窗口所需的初始坐标。
     */
    void mousePressEvent(QMouseEvent *event) override;

    /*
     * 处理鼠标移动事件。
     * 左键拖动标题栏时移动顶层窗口。
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /*
     * 处理鼠标双击事件。
     * 双击标题栏时切换最大化和还原状态。
     */
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    /*
     * 初始化标题栏界面。
     * 创建标题、主题按钮和窗口控制按钮。
     */
    void setupUi();

    /*
     * 切换顶层窗口最大化状态。
     * 当前为最大化时还原，否则最大化。
     */
    void toggleMaximized();

    /*
     * 更新最大化按钮状态属性。
     * maximized 表示当前窗口是否处于最大化状态，用于刷新主题图标。
     */
    void updateMaximizeButtonState(bool maximized);

    QPoint m_dragPosition;
    QLabel *m_titleLabel = nullptr;
    QPushButton *m_themeButton = nullptr;
    QPushButton *m_minimizeButton = nullptr;
    QPushButton *m_maximizeButton = nullptr;
    QPushButton *m_closeButton = nullptr;
};

#endif // TITLEBAR_H
