#include "WindowManager.h"

#include <QSplitter>
#include <QTabWidget>
#include <QVBoxLayout>

WindowManager::WindowManager(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void WindowManager::registerWindow(const WindowDescriptor &descriptor)
{
    if (!descriptor.widget) {
        return;
    }

    if (descriptor.area == WindowArea::Floating) {
        registerFloatingWindow(descriptor);
        return;
    }

    QTabWidget *tabs = tabWidgetForArea(descriptor.area);
    if (!tabs) {
        return;
    }

    tabs->addTab(descriptor.widget, descriptor.title);
    m_windowCounts[descriptor.area] = m_windowCounts.value(descriptor.area) + 1;
}

int WindowManager::windowCount(WindowArea area) const
{
    return m_windowCounts.value(area);
}

void WindowManager::setupUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    m_mainSplitter->setObjectName(QStringLiteral("WindowMainSplitter"));

    m_leftTabs = new QTabWidget(m_mainSplitter);
    m_leftTabs->setObjectName(QStringLiteral("WindowAreaTabs"));

    m_centerSplitter = new QSplitter(Qt::Vertical, m_mainSplitter);
    m_centerSplitter->setObjectName(QStringLiteral("WindowCenterSplitter"));
    m_centralTabs = new QTabWidget(m_centerSplitter);
    m_centralTabs->setObjectName(QStringLiteral("WindowAreaTabs"));
    m_bottomTabs = new QTabWidget(m_centerSplitter);
    m_bottomTabs->setObjectName(QStringLiteral("WindowAreaTabs"));

    m_rightTabs = new QTabWidget(m_mainSplitter);
    m_rightTabs->setObjectName(QStringLiteral("WindowAreaTabs"));

    m_mainSplitter->addWidget(m_leftTabs);
    m_mainSplitter->addWidget(m_centerSplitter);
    m_mainSplitter->addWidget(m_rightTabs);
    m_mainSplitter->setStretchFactor(0, 0);
    m_mainSplitter->setStretchFactor(1, 1);
    m_mainSplitter->setStretchFactor(2, 0);
    m_mainSplitter->setSizes({180, 740, 220});

    m_centerSplitter->addWidget(m_centralTabs);
    m_centerSplitter->addWidget(m_bottomTabs);
    m_centerSplitter->setStretchFactor(0, 1);
    m_centerSplitter->setStretchFactor(1, 0);
    m_centerSplitter->setSizes({520, 180});

    layout->addWidget(m_mainSplitter);
}

QTabWidget *WindowManager::tabWidgetForArea(WindowArea area) const
{
    switch (area) {
    case WindowArea::Central:
        return m_centralTabs;
    case WindowArea::LeftDock:
        return m_leftTabs;
    case WindowArea::RightDock:
        return m_rightTabs;
    case WindowArea::BottomDock:
        return m_bottomTabs;
    case WindowArea::Floating:
        return nullptr;
    }

    return nullptr;
}

void WindowManager::registerFloatingWindow(const WindowDescriptor &descriptor)
{
    QWidget *window = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(window);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(descriptor.widget);
    window->setWindowTitle(descriptor.title);
    window->resize(640, 420);
    window->show();
    m_floatingWindows.append(window);
    m_windowCounts[WindowArea::Floating] = m_windowCounts.value(WindowArea::Floating) + 1;
}
