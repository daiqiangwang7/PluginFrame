#ifndef LOGVIEWERWIDGET_H
#define LOGVIEWERWIDGET_H

#include "LogService.h"

#include <QWidget>

class LogService;
class QTableWidget;

class LogViewerWidget : public QWidget
{
    Q_OBJECT

public:
    /*
     * 创建日志查看窗口。
     * logService 为框架日志服务，parent 为 Qt 对象父节点。
     */
    explicit LogViewerWidget(LogService *logService, QWidget *parent = nullptr);

private:
    /*
     * 初始化日志查看界面。
     * 创建日志表格并加载已有日志记录。
     */
    void setupUi();

    /*
     * 追加一条日志记录到表格。
     * record 为需要显示的日志记录。
     */
    void appendRecord(const LogRecord &record);

    /*
     * 日志查看器绑定的日志服务。
     */
    LogService *m_logService = nullptr;
    /*
     * 显示日志记录的表格控件。
     */
    QTableWidget *m_table = nullptr;
};

#endif // LOGVIEWERWIDGET_H
