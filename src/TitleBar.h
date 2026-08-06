#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>
#include <QString>

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

    /*
     * 设置标题栏当前主题名称。
     * themeName 用于选择皮肤按钮在深色或浅色主题下显示的图标。
     */
    void setThemeName(const QString &themeName);

    /*
     * 设置皮肤切换按钮是否可见。
     * visible 为 false 时隐藏皮肤按钮，适用于插件浮动窗口标题栏。
     */
    void setThemeButtonVisible(bool visible);

signals:
    /*
     * 请求切换应用主题。
     * 用户点击主题按钮时发出该信号。
     */
    void themeToggleRequested();

    /*
     * 标题栏图标资源刷新完成。
     * 当主题变化导致皮肤按钮图标重设后发出，便于后续扩展联动。
     */
    void themeIconChanged();

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

    /*
     * 鼠标拖动窗口时记录的偏移坐标。
     */
    QPoint m_dragPosition;
    /*
     * 当前主题名称，用于选择标题栏按钮图标。
     */
    QString m_themeName = QStringLiteral("cyber-dark");
    /*
     * 标题栏左侧标题文本标签。
     */
    QLabel *m_titleLabel = nullptr;
    /*
     * 皮肤切换按钮。
     */
    QPushButton *m_themeButton = nullptr;
    /*
     * 窗口最小化按钮。
     */
    QPushButton *m_minimizeButton = nullptr;
    /*
     * 窗口最大化或还原按钮。
     */
    QPushButton *m_maximizeButton = nullptr;
    /*
     * 窗口关闭按钮。
     */
    QPushButton *m_closeButton = nullptr;
};

#endif // TITLEBAR_H
