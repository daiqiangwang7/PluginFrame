#include "TimeServicePlugin.h"

#include "CapabilityRegistry.h"
#include "IPluginContext.h"
#include "MessageBus.h"
#include "PluginSettings.h"

#include <QDateTime>
#include <QTimer>
#include <QtGlobal>

TimeServicePlugin::TimeServicePlugin(QObject *parent)
    : IServicePlugin(parent)
{
}

TimeServicePlugin::~TimeServicePlugin()
{
    stop();
}

void TimeServicePlugin::setContext(IPluginContext *context)
{
    m_context = context;
}

bool TimeServicePlugin::initialize()
{
    if (m_timer) {
        return true;
    }

    m_timer = new QTimer(this);
    int intervalMs = 1000;
    if (m_context && m_context->pluginSettings()) {
        const QString pluginId = QStringLiteral("com.pluginframe.time-service");
        if (!m_context->pluginSettings()->contains(pluginId, QStringLiteral("timer/intervalMs"))) {
            m_context->pluginSettings()->setValue(pluginId, QStringLiteral("timer/intervalMs"), intervalMs);
        }
        intervalMs = m_context->pluginSettings()->value(pluginId, QStringLiteral("timer/intervalMs"), intervalMs).toInt();
    }

    m_timer->setInterval(qMax(100, intervalMs));
    connect(m_timer, &QTimer::timeout, this, &TimeServicePlugin::publishTick);

    return true;
}

void TimeServicePlugin::start()
{
    if (!m_timer) {
        return;
    }

    if (m_context && m_context->capabilityRegistry()) {
        m_context->capabilityRegistry()->registerCapability({
            QStringLiteral("com.pluginframe.time-service"),
            QStringLiteral("time.tick.service"),
            QStringLiteral("service"),
            QStringLiteral("时间发布服务"),
            QStringLiteral("time.tick")
        });
    }

    publishTick();
    m_timer->start();
}

void TimeServicePlugin::stop()
{
    if (m_timer) {
        m_timer->stop();
    }
}

QString TimeServicePlugin::name() const
{
    return QStringLiteral("TimeServicePlugin");
}

void TimeServicePlugin::publishTick()
{
    if (!m_context || !m_context->messageBus()) {
        return;
    }

    const QString timeText = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    m_context->messageBus()->publish(QStringLiteral("time.tick"),
                                     {{QStringLiteral("time"), timeText}});
}
