#ifndef PLUGININSPECTORWIDGET_H
#define PLUGININSPECTORWIDGET_H

#include "PluginManager.h"

#include <QWidget>

class QTableWidget;

class PluginInspectorWidget : public QWidget
{
    Q_OBJECT

public:
    /*
     * 创建插件诊断面板。
     * parent 为 Qt 对象父节点，用于纳入 Qt 对象生命周期管理。
     */
    explicit PluginInspectorWidget(QWidget *parent = nullptr);

    /*
     * 设置需要展示的插件记录。
     * records 为插件管理器返回的插件记录快照。
     */
    void setPluginRecords(const QList<PluginRecord> &records);

private:
    /*
     * 初始化诊断面板界面。
     * 创建表格并设置列标题和基础显示属性。
     */
    void setupUi();

    /*
     * 填充单行插件记录。
     * row 为表格行号，record 为需要展示的插件记录。
     */
    void fillRecordRow(int row, const PluginRecord &record);

    QTableWidget *m_table = nullptr;
};

#endif // PLUGININSPECTORWIDGET_H
