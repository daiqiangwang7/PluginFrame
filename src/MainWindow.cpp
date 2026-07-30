#include "MainWindow.h"

#include "AppConfig.h"
#include "CyberSidebar.h"
#include "IViewPlugin.h"
#include "MessageBus.h"
#include "PluginInspectorWidget.h"
#include "PluginManager.h"
#include "StatusBarWidget.h"
#include "ThemeManager.h"
#include "TitleBar.h"
#include "WindowManager.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QResizeEvent>
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
    const AppConfig config = loadAppConfig();
    m_themeManager->applyTheme(qApp, config.defaultTheme);

    m_shellRoot = new QWidget(this);
    m_shellRoot->setObjectName(QStringLiteral("ShellRoot"));
    QVBoxLayout *rootLayout = new QVBoxLayout(m_shellRoot);
    rootLayout->setContentsMargins(1, 1, 1, 1);
    rootLayout->setSpacing(0);

    m_titleBar = new TitleBar(m_shellRoot);
    m_titleBar->setTitle(QStringLiteral("插件框架"));
    connect(m_titleBar, &TitleBar::drawerToggleRequested, this, &MainWindow::toggleFunctionDrawer);
    connect(m_titleBar, &TitleBar::themeToggleRequested, this, &MainWindow::toggleTheme);

    m_windowManager = new WindowManager(m_shellRoot);
    m_sidebar = new CyberSidebar(m_shellRoot);
    m_statusBar = new StatusBarWidget(m_shellRoot);

    QWidget *workspace = new QWidget(m_shellRoot);
    workspace->setObjectName(QStringLiteral("Workspace"));
    QHBoxLayout *workspaceLayout = new QHBoxLayout(workspace);
    workspaceLayout->setContentsMargins(0, 0, 0, 0);
    workspaceLayout->setSpacing(0);
    workspaceLayout->addWidget(m_windowManager, 1);

    rootLayout->addWidget(m_titleBar);
    rootLayout->addWidget(workspace, 1);
    rootLayout->addWidget(m_statusBar);

    setCentralWidget(m_shellRoot);
    setWindowTitle(QStringLiteral("插件框架"));
    resize(1100, 700);
    m_sidebar->updateDrawerGeometry();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (m_sidebar) {
        m_sidebar->updateDrawerGeometry();
    }
}

void MainWindow::loadPlugins()
{
    const AppConfig config = loadAppConfig();
    m_pluginManager = new PluginManager(this);
    for (auto it = config.pluginSettings.cbegin(); it != config.pluginSettings.cend(); ++it) {
        m_pluginManager->setPluginEnabled(it.key(), it.value().enabled);
    }

    const QString pluginsPath = QApplication::applicationDirPath() + QStringLiteral("/plugins");
    m_pluginManager->loadPlugins(pluginsPath);

    const auto views = m_pluginManager->findPlugins<IViewPlugin>();
    for (IViewPlugin *view : views) {
        if (QWidget *viewWidget = view->widget()) {
            WindowArea area = WindowArea::Central;
            const auto records = m_pluginManager->pluginRecords();
            for (const PluginRecord &record : records) {
                if (record.plugin == view) {
                    area = config.pluginArea(record.metadata.id);
                    break;
                }
            }

            m_windowManager->registerWindow({view->name(),
                                             view->displayName(),
                                             area,
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

    updateStatusBar();
}

void MainWindow::toggleTheme()
{
    if (m_themeManager) {
        m_themeManager->toggleTheme(qApp);
        updateStatusBar();
    }
}

AppConfig MainWindow::loadAppConfig() const
{
    QString errorString;
    const QString filePath = QApplication::applicationDirPath() + QStringLiteral("/config/app.xml");
    return AppConfig::fromFile(filePath, &errorString);
}

void MainWindow::updateStatusBar()
{
    if (!m_statusBar || !m_themeManager || !m_pluginManager) {
        return;
    }

    m_statusBar->updateStatus(m_themeManager->currentTheme(), m_pluginManager->pluginRecords());
}

void MainWindow::toggleFunctionDrawer()
{
    if (m_sidebar) {
        m_sidebar->toggleDrawer();
    }
}
