#include "WindowManager.h"

#include "TitleBar.h"

#include <QFrame>
#include <QSplitter>
#include <QTabWidget>
#include <QVBoxLayout>

namespace {

class FloatingWindowFrame : public QWidget
{
public:
    explicit FloatingWindowFrame(const WindowDescriptor &descriptor, const QString &themeName)
    {
        setObjectName(QStringLiteral("FloatingWindowFrame"));
        setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
        resize(640, 420);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(1, 1, 1, 1);
        layout->setSpacing(0);

        TitleBar *titleBar = new TitleBar(this);
        titleBar->setTitle(descriptor.title);
        titleBar->setThemeName(themeName);
        titleBar->setThemeButtonVisible(false);

        QFrame *separator = new QFrame(this);
        separator->setObjectName(QStringLiteral("TitleSeparator"));
        separator->setFixedHeight(1);
        separator->setFrameShape(QFrame::NoFrame);

        layout->addWidget(titleBar);
        layout->addWidget(separator);
        layout->addWidget(descriptor.widget, 1);
        setWindowTitle(descriptor.title);
    }
};

} // namespace

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
    m_registeredWindows.insert(descriptor.id, qMakePair(tabs, descriptor.widget));
    m_windowCounts[descriptor.area] = m_windowCounts.value(descriptor.area) + 1;
}

int WindowManager::windowCount(WindowArea area) const
{
    return m_windowCounts.value(area);
}

bool WindowManager::activateWindow(const QString &id)
{
    if (m_floatingWindows.contains(id)) {
        QWidget *window = m_floatingWindows.value(id);
        if (!window) {
            return false;
        }
        window->show();
        window->raise();
        window->activateWindow();
        return true;
    }

    if (!m_registeredWindows.contains(id)) {
        return false;
    }

    QTabWidget *tabs = m_registeredWindows.value(id).first;
    QWidget *widget = m_registeredWindows.value(id).second;
    if (!tabs || !widget) {
        return false;
    }

    const int index = tabs->indexOf(widget);
    if (index < 0) {
        return false;
    }

    tabs->setCurrentIndex(index);
    widget->setFocus();
    return true;
}

void WindowManager::setThemeName(const QString &themeName)
{
    m_themeName = themeName;
    for (QWidget *window : m_floatingWindows) {
        if (TitleBar *titleBar = window ? window->findChild<TitleBar *>() : nullptr) {
            titleBar->setThemeName(m_themeName);
            titleBar->setThemeButtonVisible(false);
        }
    }
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
    if (m_floatingWindows.contains(descriptor.id)) {
        return;
    }

    QWidget *window = new FloatingWindowFrame(descriptor, m_themeName);
    m_floatingWindows.insert(descriptor.id, window);
    m_windowCounts[WindowArea::Floating] = m_windowCounts.value(WindowArea::Floating) + 1;
}
