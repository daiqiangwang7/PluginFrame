#ifndef PLUGINSETTINGS_H
#define PLUGINSETTINGS_H

#include <QObject>
#include <QString>
#include <QVariant>

class PluginSettings : public QObject
{
    Q_OBJECT

public:
    /*
     * 创建插件配置服务。
     * basePath 为插件配置文件目录，parent 为 Qt 对象父节点。
     */
    explicit PluginSettings(const QString &basePath = QString(), QObject *parent = nullptr);

    /*
     * 设置插件配置文件目录。
     * basePath 为配置文件保存目录，不存在时会在写入配置前自动创建。
     */
    void setBasePath(const QString &basePath);

    /*
     * 获取插件配置文件目录。
     * 返回当前用于保存插件私有配置的目录路径。
     */
    QString basePath() const;

    /*
     * 读取插件配置值。
     * pluginId 为插件唯一标识，key 为配置键，defaultValue 为缺省值。
     */
    QVariant value(const QString &pluginId, const QString &key, const QVariant &defaultValue = QVariant()) const;

    /*
     * 写入插件配置值。
     * pluginId 为插件唯一标识，key 为配置键，value 为需要保存的配置值。
     */
    void setValue(const QString &pluginId, const QString &key, const QVariant &value);

    /*
     * 判断插件配置键是否存在。
     * pluginId 为插件唯一标识，key 为配置键，存在时返回 true。
     */
    bool contains(const QString &pluginId, const QString &key) const;

    /*
     * 删除插件配置键。
     * pluginId 为插件唯一标识，key 为配置键。
     */
    void remove(const QString &pluginId, const QString &key);

    /*
     * 获取插件配置文件路径。
     * pluginId 为插件唯一标识，返回对应 ini 配置文件路径。
     */
    QString pluginFilePath(const QString &pluginId) const;

private:
    /*
     * 生成安全的配置文件名。
     * pluginId 为插件唯一标识，返回可用于文件名的字符串。
     */
    QString safePluginFileName(const QString &pluginId) const;

    /*
     * 插件私有配置文件保存目录。
     */
    QString m_basePath;
};

#endif // PLUGINSETTINGS_H
