#include "MainWindow.h"

#include "IViewPlugin.h"
#include "MessageBus.h"
#include "PluginManager.h"

#include <QApplication>
#include <QTabWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    loadPlugins();
}

void MainWindow::setupUi()
{
    m_tabs = new QTabWidget(this);
    setCentralWidget(m_tabs);
    setWindowTitle(QStringLiteral("Qt5 Plugin Framework Demo"));
    resize(800, 600);
}

void MainWindow::loadPlugins()
{
    m_pluginManager = new PluginManager(this);
    const QString pluginsPath = QApplication::applicationDirPath() + QStringLiteral("/plugins");
    m_pluginManager->loadPlugins(pluginsPath);

    const auto views = m_pluginManager->findPlugins<IViewPlugin>();
    for (IViewPlugin *view : views) {
        if (QWidget *viewWidget = view->widget()) {
            m_tabs->addTab(viewWidget, view->displayName());
        }
    }

    if (m_pluginManager->messageBus()) {
        m_pluginManager->messageBus()->publish(
            QStringLiteral("app.status"),
            {{QStringLiteral("text"), QStringLiteral("Host loaded HelloPlugin through MessageBus")}});
    }
}
