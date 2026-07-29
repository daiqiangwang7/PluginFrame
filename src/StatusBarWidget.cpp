#include "StatusBarWidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QTime>

QString formatStatusSummary(const QString &themeName, const QList<PluginRecord> &records)
{
    int startedCount = 0;
    int serviceCount = 0;
    for (const PluginRecord &record : records) {
        if (record.state == PluginState::Started) {
            ++startedCount;
        }
        if (record.metadata.type == QStringLiteral("service")) {
            ++serviceCount;
        }
    }

    return QStringLiteral("主题：%1   插件：%2   服务：%3   时间：%4")
            .arg(themeName)
            .arg(startedCount)
            .arg(serviceCount)
            .arg(QTime::currentTime().toString(QStringLiteral("HH:mm:ss")));
}

StatusBarWidget::StatusBarWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void StatusBarWidget::updateStatus(const QString &themeName, const QList<PluginRecord> &records)
{
    m_summaryLabel->setText(formatStatusSummary(themeName, records));
}

void StatusBarWidget::setupUi()
{
    setObjectName(QStringLiteral("CyberStatusBar"));
    setFixedHeight(30);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 0, 12, 0);
    m_summaryLabel = new QLabel(QStringLiteral("主题：cyber-dark   插件：0   服务：0"), this);
    m_summaryLabel->setObjectName(QStringLiteral("StatusSummary"));
    layout->addWidget(m_summaryLabel);
    layout->addStretch();
}
