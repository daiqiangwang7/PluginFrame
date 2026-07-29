#include "TimeServicePlugin.h"

#include "IPluginContext.h"
#include "MessageBus.h"

#include <QDateTime>
#include <QTimer>

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
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &TimeServicePlugin::publishTick);

    return true;
}

void TimeServicePlugin::start()
{
    if (!m_timer) {
        return;
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
