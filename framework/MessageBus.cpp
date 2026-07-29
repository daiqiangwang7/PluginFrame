#include "MessageBus.h"

MessageBus::MessageBus(QObject *parent)
    : QObject(parent)
{
}

void MessageBus::publish(const QString &topic, const QVariantMap &payload)
{
    emit messagePublished(topic, payload);
}

QMetaObject::Connection MessageBus::subscribe(const QString &topic,
                                              QObject *receiver,
                                              const MessageHandler &handler)
{
    return connect(this, &MessageBus::messagePublished, receiver,
                   [topic, handler](const QString &publishedTopic,
                                    const QVariantMap &payload) {
        if (publishedTopic == topic && handler) {
            handler(publishedTopic, payload);
        }
    });
}
