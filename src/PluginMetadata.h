#ifndef PLUGINMETADATA_H
#define PLUGINMETADATA_H

#include <QJsonObject>
#include <QString>
#include <QStringList>

struct PluginMetadata
{
    /*
     * 插件唯一标识。
     */
    QString id;
    /*
     * 插件内部名称。
     */
    QString name;
    /*
     * 插件显示名称。
     */
    QString displayName;
    /*
     * 插件版本号。
     */
    QString version;
    /*
     * 插件类型，当前支持 view 和 service。
     */
    QString type;
    /*
     * 插件视图区域配置文本。
     */
    QString area;
    /*
     * 插件依赖的其他插件 ID 列表。
     */
    QStringList dependencies;
    /*
     * 插件是否启用。
     */
    bool enabled = true;
    /*
     * 元数据中是否显式配置了窗口区域。
     */
    bool hasArea = false;

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
