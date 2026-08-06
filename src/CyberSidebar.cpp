#include "CyberSidebar.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
#include <QToolButton>
#include <QVBoxLayout>

namespace {

const int kCollapsedSize = 34;
const int kExpandedWidth = 226;
const int kExpandedHeight = 326;
const int kTitleBarHeight = 32;
const int kStatusBarHeight = 30;
const int kEntriesPerPage = 10;

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
    const int availableHeight = parentWidget()->height() - kTitleBarHeight - kStatusBarHeight;
    const int drawerHeight = m_expanded ? qMin(kExpandedHeight, availableHeight) : kCollapsedSize;
    const int drawerTop = kTitleBarHeight + qMax(0, (availableHeight - drawerHeight) / 2);

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

void CyberSidebar::addWindowEntry(const QString &windowId, const QString &text)
{
    DrawerEntry entry;
    entry.id = windowId;
    entry.text = text;
    entry.iconName = QStringLiteral("window");
    entry.windowEntry = true;
    m_entries.append(entry);
    rebuildEntryButtons();
}

void CyberSidebar::setThemeName(const QString &themeName)
{
    m_themeName = themeName;
    rebuildEntryButtons();
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
    m_handleButton->setFixedSize(kCollapsedSize, kCollapsedSize);
    m_handleButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    rootLayout->addWidget(m_handleButton, 0, Qt::AlignVCenter);

    m_contentWidget = new QWidget(this);
    m_contentWidget->setObjectName(QStringLiteral("DrawerContent"));
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(8, 8, 8, 8);
    m_contentLayout->setSpacing(6);
    rootLayout->addWidget(m_contentWidget, 1);

    m_entriesWidget = new QWidget(m_contentWidget);
    m_entriesWidget->setObjectName(QStringLiteral("DrawerEntries"));
    m_entriesLayout = new QGridLayout(m_entriesWidget);
    m_entriesLayout->setContentsMargins(0, 0, 0, 0);
    m_entriesLayout->setHorizontalSpacing(4);
    m_entriesLayout->setVerticalSpacing(6);
    m_contentLayout->addWidget(m_entriesWidget);

    m_pagerWidget = new QWidget(m_contentWidget);
    m_pagerWidget->setObjectName(QStringLiteral("DrawerPager"));
    QHBoxLayout *pagerLayout = new QHBoxLayout(m_pagerWidget);
    pagerLayout->setContentsMargins(0, 0, 0, 0);
    pagerLayout->setSpacing(6);
    m_previousButton = new QPushButton(QStringLiteral("<"), m_pagerWidget);
    m_previousButton->setObjectName(QStringLiteral("DrawerPageButton"));
    m_nextButton = new QPushButton(QStringLiteral(">"), m_pagerWidget);
    m_nextButton->setObjectName(QStringLiteral("DrawerPageButton"));
    m_pageLabel = new QLabel(QStringLiteral("1/1"), m_pagerWidget);
    m_pageLabel->setObjectName(QStringLiteral("DrawerPageLabel"));
    m_pageLabel->setAlignment(Qt::AlignCenter);
    pagerLayout->addStretch();
    pagerLayout->addWidget(m_previousButton);
    pagerLayout->addWidget(m_pageLabel);
    pagerLayout->addWidget(m_nextButton);
    pagerLayout->addStretch();
    m_contentLayout->addWidget(m_pagerWidget);

    addPageEntry(QStringLiteral("总览"), QStringLiteral("overview"), QStringLiteral("overview"));
    addPageEntry(QStringLiteral("插件"), QStringLiteral("plugins"), QStringLiteral("plugins"));
    addPageEntry(QStringLiteral("服务"), QStringLiteral("services"), QStringLiteral("services"));
    addPageEntry(QStringLiteral("日志"), QStringLiteral("logs"), QStringLiteral("logs"));
    addPageEntry(QStringLiteral("设置"), QStringLiteral("settings"), QStringLiteral("settings"));

    connect(m_handleButton, &QPushButton::clicked, this, &CyberSidebar::toggleDrawer);
    connect(m_previousButton, &QPushButton::clicked, this, &CyberSidebar::previousPage);
    connect(m_nextButton, &QPushButton::clicked, this, &CyberSidebar::nextPage);
    rebuildEntryButtons();
}

void CyberSidebar::addPageEntry(const QString &text, const QString &pageId, const QString &iconName)
{
    DrawerEntry entry;
    entry.id = pageId;
    entry.text = text;
    entry.iconName = iconName;
    entry.windowEntry = false;
    m_entries.append(entry);
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
        m_handleButton->setFixedSize(kCollapsedSize, kCollapsedSize);
    }

    setFixedWidth(m_expanded ? kExpandedWidth : kCollapsedSize);
    rebuildEntryButtons();
}

void CyberSidebar::rebuildEntryButtons()
{
    if (!m_entriesLayout || !m_pageLabel || !m_previousButton || !m_nextButton) {
        return;
    }

    qDeleteAll(m_entryButtons);
    m_entryButtons.clear();

    const int pageCount = qMax(1, (m_entries.size() + kEntriesPerPage - 1) / kEntriesPerPage);
    m_currentPage = qBound(0, m_currentPage, pageCount - 1);
    const int start = m_currentPage * kEntriesPerPage;
    const int end = qMin(start + kEntriesPerPage, m_entries.size());

    for (int i = start; i < end; ++i) {
        const DrawerEntry entry = m_entries.at(i);
        QToolButton *button = new QToolButton(m_entriesWidget);
        button->setObjectName(QStringLiteral("SidebarToolButton"));
        button->setText(entry.text);
        button->setIcon(QIcon(drawerIconPath(entry.iconName)));
        button->setIconSize(QSize(24, 24));
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setFixedSize(88, 52);
        connect(button, &QToolButton::clicked, this, [this, entry]() {
            if (entry.windowEntry) {
                emit windowRequested(entry.id);
            } else {
                emit pageRequested(entry.id);
            }
        });
        const int pageIndex = i - start;
        m_entriesLayout->addWidget(button, pageIndex / 2, pageIndex % 2);
        m_entryButtons.append(button);
    }

    m_pageLabel->setText(QStringLiteral("%1/%2").arg(m_currentPage + 1).arg(pageCount));
    m_pagerWidget->setVisible(pageCount > 1);
    m_previousButton->setEnabled(m_currentPage > 0);
    m_nextButton->setEnabled(m_currentPage < pageCount - 1);
}

void CyberSidebar::previousPage()
{
    if (m_currentPage > 0) {
        --m_currentPage;
        rebuildEntryButtons();
    }
}

void CyberSidebar::nextPage()
{
    const int pageCount = qMax(1, (m_entries.size() + kEntriesPerPage - 1) / kEntriesPerPage);
    if (m_currentPage < pageCount - 1) {
        ++m_currentPage;
        rebuildEntryButtons();
    }
}

QString CyberSidebar::drawerIconPath(const QString &iconName) const
{
    const QString folder = m_themeName == QStringLiteral("cyber-light")
            ? QStringLiteral("light")
            : QStringLiteral("dark");
    return QStringLiteral(":/icons/drawer/%1/%2.png").arg(folder, iconName);
}
