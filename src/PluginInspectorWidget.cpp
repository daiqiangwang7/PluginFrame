#include "PluginInspectorWidget.h"

#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

namespace {

QTableWidgetItem *createReadOnlyItem(const QString &text)
{
    QTableWidgetItem *item = new QTableWidgetItem(text);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    return item;
}

} // namespace

PluginInspectorWidget::PluginInspectorWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void PluginInspectorWidget::setPluginRecords(const QList<PluginRecord> &records)
{
    m_table->setRowCount(records.size());
    for (int row = 0; row < records.size(); ++row) {
        fillRecordRow(row, records.at(row));
    }
    m_table->resizeColumnsToContents();
}

void PluginInspectorWidget::setupUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_table = new QTableWidget(this);
    m_table->setColumnCount(7);
    m_table->setHorizontalHeaderLabels({
        QStringLiteral("名称"),
        QStringLiteral("类型"),
        QStringLiteral("版本"),
        QStringLiteral("依赖"),
        QStringLiteral("状态"),
        QStringLiteral("路径"),
        QStringLiteral("错误")
    });
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    layout->addWidget(m_table);
}

void PluginInspectorWidget::fillRecordRow(int row, const PluginRecord &record)
{
    m_table->setItem(row, 0, createReadOnlyItem(record.name));
    m_table->setItem(row, 1, createReadOnlyItem(record.metadata.type));
    m_table->setItem(row, 2, createReadOnlyItem(record.metadata.version));
    m_table->setItem(row, 3, createReadOnlyItem(record.metadata.dependencies.join(QStringLiteral(", "))));
    m_table->setItem(row, 4, createReadOnlyItem(pluginStateToString(record.state)));
    m_table->setItem(row, 5, createReadOnlyItem(record.filePath));
    m_table->setItem(row, 6, createReadOnlyItem(record.errorString));
}
