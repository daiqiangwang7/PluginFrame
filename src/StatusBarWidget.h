#ifndef STATUSBARWIDGET_H
#define STATUSBARWIDGET_H

#include "PluginManager.h"

#include <QWidget>

class QLabel;

/*
 * 格式化工作台状态摘要。
 * themeName 为当前主题名称，records 为插件记录快照。
 */
QString formatStatusSummary(const QString &themeName, const QList<PluginRecord> &records);

class StatusBarWidget : public QWidget
{
    Q_OBJECT

public:
    /*
     * 创建状态栏部件。
     * parent 为 Qt 对象父节点，用于纳入 Qt 对象生命周期管理。
     */
    explicit StatusBarWidget(QWidget *parent = nullptr);

    /*
     * 更新状态栏显示信息。
     * themeName 为当前主题名称，records 为插件记录快照。
     */
    void updateStatus(const QString &themeName, const QList<PluginRecord> &records);

private:
    /*
     * 初始化状态栏界面。
     * 创建摘要文本标签并设置基础布局。
     */
    void setupUi();

    QLabel *m_summaryLabel = nullptr;
};

#endif // STATUSBARWIDGET_H
