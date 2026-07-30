#include "HelloPlugin.h"

#include "CapabilityRegistry.h"
#include "IPluginContext.h"
#include "MessageBus.h"
#include "PluginSettings.h"

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
    QString greeting = QStringLiteral("你好，来自视图插件！");
    if (m_context && m_context->pluginSettings()) {
        const QString pluginId = QStringLiteral("com.pluginframe.hello");
        if (!m_context->pluginSettings()->contains(pluginId, QStringLiteral("ui/greeting"))) {
            m_context->pluginSettings()->setValue(pluginId, QStringLiteral("ui/greeting"), greeting);
        }
        greeting = m_context->pluginSettings()->value(pluginId, QStringLiteral("ui/greeting"), greeting).toString();
    }

    m_label = new QLabel(greeting, m_widget);
    m_label->setAlignment(Qt::AlignCenter);
    m_timeLabel = new QLabel(QStringLiteral("等待时间服务..."), m_widget);
    m_timeLabel->setAlignment(Qt::AlignCenter);

    QPushButton *button = new QPushButton(QStringLiteral("发布 hello.clicked 消息"), m_widget);
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
    if (!m_context) {
        return;
    }

    if (m_context->capabilityRegistry()) {
        m_context->capabilityRegistry()->registerCapability({
            QStringLiteral("com.pluginframe.hello"),
            QStringLiteral("hello.view"),
            QStringLiteral("view"),
            QStringLiteral("你好插件视图"),
            QStringLiteral("HelloPlugin")
        });
    }

    if (!m_context->messageBus()) {
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
                m_timeLabel->setText(QStringLiteral("时间服务：%1").arg(timeText));
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
    return QStringLiteral("你好插件");
}
