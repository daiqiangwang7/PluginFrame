#include "CyberSidebar.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QVBoxLayout>

namespace {

const int kCollapsedSize = 34;
const int kExpandedWidth = 220;
const int kTitleBarHeight = 42;
const int kStatusBarHeight = 30;
const int kCollapsedTopOffset = 18;

} // namespace

CyberSidebar::CyberSidebar(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    updateExpandedState();
}

void CyberSidebar::toggleDrawer()
{
    m_expanded = !m_expanded;
    updateExpandedState();
    updateDrawerGeometry();
    raise();
}

void CyberSidebar::updateDrawerGeometry()
{
    if (!parentWidget()) {
        return;
    }

    const int drawerWidth = m_expanded ? kExpandedWidth : kCollapsedSize;
    const int drawerHeight = m_expanded
        ? parentWidget()->height() - kTitleBarHeight - kStatusBarHeight
        : kCollapsedSize;
    const int drawerTop = m_expanded ? kTitleBarHeight : kTitleBarHeight + kCollapsedTopOffset;

    setGeometry(parentWidget()->width() - drawerWidth,
                drawerTop,
                drawerWidth,
                drawerHeight);
    show();
    raise();
}

bool CyberSidebar::isExpanded() const
{
    return m_expanded;
}

void CyberSidebar::setupUi()
{
    setObjectName(QStringLiteral("CyberDrawer"));
    setMinimumWidth(kCollapsedSize);
    setMaximumWidth(kExpandedWidth);

    QHBoxLayout *rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    m_handleButton = new QPushButton(QStringLiteral("≡"), this);
    m_handleButton->setObjectName(QStringLiteral("DrawerHandleButton"));
    m_handleButton->setToolTip(QStringLiteral("展开功能抽屉"));
    m_handleButton->setFixedWidth(kCollapsedSize);
    m_handleButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    rootLayout->addWidget(m_handleButton);

    m_contentWidget = new QWidget(this);
    m_contentWidget->setObjectName(QStringLiteral("DrawerContent"));
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(10, 14, 10, 14);
    m_contentLayout->setSpacing(10);
    rootLayout->addWidget(m_contentWidget, 1);

    addNavigationButton(QStringLiteral("总览"), QStringLiteral("overview"));
    addNavigationButton(QStringLiteral("插件"), QStringLiteral("plugins"));
    addNavigationButton(QStringLiteral("服务"), QStringLiteral("services"));
    addNavigationButton(QStringLiteral("日志"), QStringLiteral("logs"));
    addNavigationButton(QStringLiteral("设置"), QStringLiteral("settings"));

    m_contentLayout->addStretch();

    connect(m_handleButton, &QPushButton::clicked, this, &CyberSidebar::toggleDrawer);
}

void CyberSidebar::addNavigationButton(const QString &text, const QString &pageId)
{
    QPushButton *button = new QPushButton(text, m_contentWidget);
    button->setObjectName(QStringLiteral("SidebarButton"));
    button->setMinimumHeight(38);
    connect(button, &QPushButton::clicked, this, [this, pageId]() {
        emit pageRequested(pageId);
    });
    m_contentLayout->addWidget(button);
}

void CyberSidebar::updateExpandedState()
{
    if (m_contentWidget) {
        m_contentWidget->setVisible(m_expanded);
    }

    if (m_handleButton) {
        m_handleButton->setText(m_expanded ? QStringLiteral("×") : QStringLiteral("≡"));
        m_handleButton->setToolTip(m_expanded ? QStringLiteral("收起功能抽屉")
                                               : QStringLiteral("展开功能抽屉"));
        if (m_expanded) {
            m_handleButton->setMinimumSize(kCollapsedSize, 0);
            m_handleButton->setMaximumSize(kCollapsedSize, QWIDGETSIZE_MAX);
        } else {
            m_handleButton->setFixedSize(kCollapsedSize, kCollapsedSize);
        }
    }

    setFixedWidth(m_expanded ? kExpandedWidth : kCollapsedSize);
}
