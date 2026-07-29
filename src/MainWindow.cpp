#include "MainWindow.h"

#include "IViewPlugin.h"
#include "MessageBus.h"
#include "PluginInspectorWidget.h"
#include "PluginManager.h"
#include "ThemeManager.h"
#include "TitleBar.h"
#include "WindowManager.h"

#include <QApplication>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    loadPlugins();
}

void MainWindow::setupUi()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground, false);

    m_themeManager = new ThemeManager(this);
    m_themeManager->applyTheme(qApp, QStringLiteral("cyber-dark"));

    m_shellRoot = new QWidget(this);
    m_shellRoot->setObjectName(QStringLiteral("ShellRoot"));
    QVBoxLayout *rootLayout = new QVBoxLayout(m_shellRoot);
    rootLayout->setContentsMargins(1, 1, 1, 1);
    rootLayout->setSpacing(0);

    m_titleBar = new TitleBar(m_shellRoot);
    m_titleBar->setTitle(QStringLiteral("插件框架"));
    connect(m_titleBar, &TitleBar::themeToggleRequested, this, &MainWindow::toggleTheme);

    m_windowManager = new WindowManager(m_shellRoot);

    rootLayout->addWidget(m_titleBar);
    rootLayout->addWidget(m_windowManager, 1);

    setCentralWidget(m_shellRoot);
    setWindowTitle(QStringLiteral("插件框架"));
    resize(1100, 700);
}

void MainWindow::loadPlugins()
{
    m_pluginManager = new PluginManager(this);
    const QString pluginsPath = QApplication::applicationDirPath() + QStringLiteral("/plugins");
    m_pluginManager->loadPlugins(pluginsPath);

    const auto views = m_pluginManager->findPlugins<IViewPlugin>();
    for (IViewPlugin *view : views) {
        if (QWidget *viewWidget = view->widget()) {
            m_windowManager->registerWindow({view->name(),
                                             view->displayName(),
                                             WindowArea::Central,
                                             viewWidget});
        }
    }

    PluginInspectorWidget *inspector = new PluginInspectorWidget(this);
    inspector->setPluginRecords(m_pluginManager->pluginRecords());
    m_windowManager->registerWindow({QStringLiteral("plugin.inspector"),
                                     QStringLiteral("插件诊断"),
                                     WindowArea::BottomDock,
                                     inspector});

    if (m_pluginManager->messageBus()) {
        m_pluginManager->messageBus()->publish(
            QStringLiteral("app.status"),
            {{QStringLiteral("text"), QStringLiteral("宿主已通过消息总线加载插件")}});
    }
}

void MainWindow::toggleTheme()
{
    if (m_themeManager) {
        m_themeManager->toggleTheme(qApp);
    }
}
