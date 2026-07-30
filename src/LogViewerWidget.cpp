#include "LogViewerWidget.h"

#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

namespace {

QTableWidgetItem *createLogItem(const QString &text)
{
    QTableWidgetItem *item = new QTableWidgetItem(text);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    return item;
}

} // namespace

LogViewerWidget::LogViewerWidget(LogService *logService, QWidget *parent)
    : QWidget(parent)
    , m_logService(logService)
{
    setupUi();
}

void LogViewerWidget::setupUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(0);

    m_table = new QTableWidget(this);
    m_table->setObjectName(QStringLiteral("LogTable"));
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({
        QStringLiteral("时间"),
        QStringLiteral("等级"),
        QStringLiteral("来源"),
        QStringLiteral("内容")
    });
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    layout->addWidget(m_table);

    if (!m_logService) {
        return;
    }

    for (const LogRecord &record : m_logService->records()) {
        appendRecord(record);
    }

    connect(m_logService, &LogService::recordAdded, this, &LogViewerWidget::appendRecord);
}

void LogViewerWidget::appendRecord(const LogRecord &record)
{
    const int row = m_table->rowCount();
    m_table->insertRow(row);
    m_table->setItem(row, 0, createLogItem(record.timestamp.toString(QStringLiteral("HH:mm:ss.zzz"))));
    m_table->setItem(row, 1, createLogItem(logLevelToString(record.level)));
    m_table->setItem(row, 2, createLogItem(record.source));
    m_table->setItem(row, 3, createLogItem(record.message));
    m_table->scrollToBottom();
}
