#ifndef CAPABILITYREGISTRY_H
#define CAPABILITYREGISTRY_H

#include <QObject>
#include <QString>
#include <QList>

/*
 * 描述插件向宿主注册的一项能力。
 * 能力可表示窗口、命令、服务、设置页或抽屉入口等可扩展资源。
 */
struct PluginCapability
{
    /*
     * 能力所属插件的唯一标识。
     */
    QString pluginId;
    /*
     * 能力自身的唯一标识。
     */
    QString id;
    /*
     * 能力类型，例如 view、service、command 或 settings。
     */
    QString type;
    /*
     * 能力在界面中的显示名称。
     */
    QString displayName;
    /*
     * 能力目标对象名称，用于宿主定位具体窗口或命令。
     */
    QString target;
};

class CapabilityRegistry : public QObject
{
    Q_OBJECT

public:
    /*
     * 创建插件能力注册中心。
     * parent 为 Qt 对象父节点，用于纳入 Qt 对象生命周期管理。
     */
    explicit CapabilityRegistry(QObject *parent = nullptr);

    /*
     * 注册插件能力。
     * capability 描述能力归属、类型、显示名称和目标标识，成功注册时返回 true。
     */
    bool registerCapability(const PluginCapability &capability);

    /*
     * 注销指定插件的全部能力。
     * pluginId 为插件唯一标识，通常在插件停止或卸载时调用。
     */
    void unregisterPluginCapabilities(const QString &pluginId);

    /*
     * 获取所有已注册能力。
     * 返回值为能力快照，可用于诊断页面和宿主界面生成。
     */
    QList<PluginCapability> capabilities() const;

    /*
     * 按类型查找能力。
     * type 为能力类型，返回所有匹配该类型的能力。
     */
    QList<PluginCapability> findCapabilities(const QString &type) const;

signals:
    /*
     * 能力注册表发生变化。
     * 插件注册或注销能力后发出，便于宿主界面刷新。
     */
    void capabilitiesChanged();

private:
    /*
     * 当前已注册的全部插件能力列表。
     */
    QList<PluginCapability> m_capabilities;
};

#endif // CAPABILITYREGISTRY_H
