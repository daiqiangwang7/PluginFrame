#include "HelloPlugin.h"

#include "IPluginContext.h"
#include "MessageBus.h"

#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

HelloPlugin::HelloPlugin(QObject *parent)
    : IViewPlugin(parent)
{
}

HelloPlugin::~HelloPlugin()
{
    stop();
}

bool HelloPlugin::initialize()
{
    if (m_widget) {
        return true;
    }

    m_widget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(m_widget);
    m_label = new QLabel(QStringLiteral("Hello from Plugin! (Qt5)"), m_widget);
    m_label->setAlignment(Qt::AlignCenter);
    m_timeLabel = new QLabel(QStringLiteral("Waiting for time service..."), m_widget);
    m_timeLabel->setAlignment(Qt::AlignCenter);

    QPushButton *button = new QPushButton(QStringLiteral("Publish hello.clicked"), m_widget);
    QObject::connect(button, &QPushButton::clicked, m_widget, [this]() {
        if (m_context && m_context->messageBus()) {
            m_context->messageBus()->publish(QStringLiteral("hello.clicked"),
                                             {{QStringLiteral("plugin"), name()}});
        }
    });

    layout->addStretch();
    layout->addWidget(m_label);
    layout->addWidget(m_timeLabel);
    layout->addWidget(button);
    layout->addStretch();

    return true;
}

void HelloPlugin::setContext(IPluginContext *context)
{
    m_context = context;
}

void HelloPlugin::start()
{
    if (!m_context || !m_context->messageBus()) {
        return;
    }

    m_context->messageBus()->subscribe(QStringLiteral("app.status"), this,
                                       [this](const QString &, const QVariantMap &payload) {
        if (m_label) {
            const QString text = payload.value(QStringLiteral("text")).toString();
            if (!text.isEmpty()) {
                m_label->setText(text);
            }
        }
    });

    m_context->messageBus()->subscribe(QStringLiteral("time.tick"), this,
                                       [this](const QString &, const QVariantMap &payload) {
        if (m_timeLabel) {
            const QString timeText = payload.value(QStringLiteral("time")).toString();
            if (!timeText.isEmpty()) {
                m_timeLabel->setText(QStringLiteral("Time service: %1").arg(timeText));
            }
        }
    });
}

void HelloPlugin::stop()
{
    delete m_widget;
    m_widget = nullptr;
    m_label = nullptr;
    m_timeLabel = nullptr;
}

QString HelloPlugin::name() const
{
    return QStringLiteral("HelloPlugin");
}

QWidget *HelloPlugin::widget()
{
    return m_widget;
}

QString HelloPlugin::displayName() const
{
    return QStringLiteral("Hello Tab");
}
