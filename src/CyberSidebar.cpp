#include "CyberSidebar.h"

#include <QPushButton>
#include <QVBoxLayout>

CyberSidebar::CyberSidebar(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void CyberSidebar::setupUi()
{
    setObjectName(QStringLiteral("CyberSidebar"));
    setFixedWidth(128);

    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(10, 14, 10, 14);
    m_layout->setSpacing(10);

    addNavigationButton(QStringLiteral("总览"), QStringLiteral("overview"));
    addNavigationButton(QStringLiteral("插件"), QStringLiteral("plugins"));
    addNavigationButton(QStringLiteral("服务"), QStringLiteral("services"));
    addNavigationButton(QStringLiteral("日志"), QStringLiteral("logs"));
    addNavigationButton(QStringLiteral("设置"), QStringLiteral("settings"));

    m_layout->addStretch();
}

void CyberSidebar::addNavigationButton(const QString &text, const QString &pageId)
{
    QPushButton *button = new QPushButton(text, this);
    button->setObjectName(QStringLiteral("SidebarButton"));
    button->setMinimumHeight(38);
    connect(button, &QPushButton::clicked, this, [this, pageId]() {
        emit pageRequested(pageId);
    });
    m_layout->addWidget(button);
}
