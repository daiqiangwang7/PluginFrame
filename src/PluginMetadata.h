#ifndef PLUGINMETADATA_H
#define PLUGINMETADATA_H

#include <QJsonObject>
#include <QString>
#include <QStringList>

struct PluginMetadata
{
    QString id;
    QString name;
    QString displayName;
    QString version;
    QString type;
    QStringList dependencies;
    bool enabled = true;

    /*
     * 从 JSON 对象解析插件元数据。
     * json 为插件元数据对象，errorString 用于返回解析失败原因。
     */
    static PluginMetadata fromJsonObject(const QJsonObject &json, QString *errorString = nullptr);

    /*
     * 判断插件元数据是否有效。
     * 返回 true 表示所有必填字段均存在且类型合法。
     */
    bool isValid() const;
};

#endif // PLUGINMETADATA_H
