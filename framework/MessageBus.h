#ifndef MESSAGEBUS_H
#define MESSAGEBUS_H

#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QVariantMap>

#include <functional>

class MessageBus : public QObject
{
    Q_OBJECT

public:
    using MessageHandler = std::function<void(const QString &, const QVariantMap &)>;

    /*
     * 创建消息总线对象。
     * parent 为 Qt 对象父节点，用于纳入 Qt 对象生命周期管理。
     */
    explicit MessageBus(QObject *parent = nullptr);

    /*
     * 发布指定主题的消息。
     * topic 为消息主题，payload 为随消息传递的键值数据。
     */
    void publish(const QString &topic, const QVariantMap &payload = QVariantMap());

    /*
     * 订阅指定主题的消息。
     * receiver 为接收者对象，handler 会在 topic 匹配时被调用。
     */
    QMetaObject::Connection subscribe(const QString &topic,
                                      QObject *receiver,
                                      const MessageHandler &handler);

signals:
    /*
     * 通知消息已经发布。
     * topic 为消息主题，payload 为随消息传递的键值数据。
     */
    void messagePublished(const QString &topic, const QVariantMap &payload);
};

#endif // MESSAGEBUS_H
