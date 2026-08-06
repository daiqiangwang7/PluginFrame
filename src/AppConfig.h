#ifndef APPCONFIG_H
#define APPCONFIG_H

#include "WindowManager.h"

#include <QHash>
#include <QString>

struct PluginConfig
{
    /*
     * 插件是否启用。
     */
    bool enabled = true;
    /*
     * 插件视图默认注册的窗口区域。
     */
    WindowArea area = WindowArea::Central;
};

struct AppConfig
{
    /*
     * 应用启动时默认使用的主题名称。
     */
    QString defaultTheme = QStringLiteral("cyber-dark");
    /*
     * 按插件 ID 保存的插件配置表。
     */
    QHash<QString, PluginConfig> pluginSettings;

    /*
     * 从 XML 字节数据解析应用配置。
     * xml 为配置文件内容，errorString 用于返回解析失败原因。
     */
    static AppConfig fromXml(const QByteArray &xml, QString *errorString = nullptr);

    /*
     * 从 XML 文件解析应用配置。
     * filePath 为配置文件路径，读取或解析失败时返回默认配置。
     */
    static AppConfig fromFile(const QString &filePath, QString *errorString = nullptr);

    /*
     * 查询插件是否启用。
     * pluginId 为插件唯一标识，未配置时默认返回 true。
     */
    bool isPluginEnabled(const QString &pluginId) const;

    /*
     * 查询插件目标窗口区域。
     * pluginId 为插件唯一标识，未配置时默认返回中心区域。
     */
    WindowArea pluginArea(const QString &pluginId) const;
};

/*
 * 将窗口区域文本转换为枚举值。
 * text 为 XML 中的区域名称，不识别时返回中心区域。
 */
WindowArea windowAreaFromString(const QString &text);

#endif // APPCONFIG_H
