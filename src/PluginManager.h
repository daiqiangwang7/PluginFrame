#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "IPlugin.h"
#include "PluginMetadata.h"

#include <QList>
#include <QHash>
#include <QObject>
#include <QSet>

class MessageBus;
class PluginContext;
class CapabilityRegistry;
class LogService;
class QPluginLoader;
class PluginSettings;

/*
 * 表示插件当前生命周期状态。
 * 用于诊断插件加载、初始化、启动、停止、禁用和失败过程。
 */
enum class PluginState
{
    /*
     * 插件动态库已加载。
     */
    Loaded,
    /*
     * 插件已完成初始化。
     */
    Initialized,
    /*
     * 插件已启动运行。
     */
    Started,
    /*
     * 插件已停止。
     */
    Stopped,
    /*
     * 插件被配置禁用。
     */
    Disabled,
    /*
     * 插件加载、初始化或依赖处理失败。
     */
    Failed
};

/*
 * 保存单个插件的运行记录。
 * 包含动态库加载器、插件实例、文件路径、名称、元数据、错误信息和生命周期状态。
 */
struct PluginRecord
{
    /*
     * 插件动态库加载器。
     */
    QPluginLoader *loader = nullptr;
    /*
     * 插件接口实例。
     */
    IPlugin *plugin = nullptr;
    /*
     * 插件动态库文件路径。
     */
    QString filePath;
    /*
     * 插件内部名称。
     */
    QString name;
    /*
     * 插件加载或启动过程中的错误信息。
     */
    QString errorString;
    /*
     * 插件元数据信息。
     */
    PluginMetadata metadata;
    /*
     * 插件当前生命周期状态。
     */
    PluginState state = PluginState::Loaded;
};

/*
 * 将插件生命周期状态转换为可读文本。
 * state 为插件状态枚举值，返回值用于日志和界面诊断显示。
 */
QString pluginStateToString(PluginState state);

class PluginManager : public QObject
{
    Q_OBJECT

public:
    /*
     * 创建插件管理器。
     * parent 为 Qt 对象父节点，用于纳入 Qt 对象生命周期管理。
     */
    explicit PluginManager(QObject *parent = nullptr);
    /*
     * 析构插件管理器。
     * 析构时会依次停止所有 Started 状态的插件。
     */
    ~PluginManager() override;

    /*
     * 从指定目录加载插件动态库。
     * path 为插件目录路径，函数会过滤动态库并启动有效插件。
     */
    void loadPlugins(const QString &path);

    /*
     * 设置插件启用状态。
     * pluginId 为插件唯一标识，enabled 为 false 时加载阶段会跳过该插件。
     */
    void setPluginEnabled(const QString &pluginId, bool enabled);

    /*
     * 获取插件管理器持有的消息总线。
     * 返回值可供宿主组件发布或订阅框架内部消息。
     */
    MessageBus *messageBus() const;

    /*
     * 获取插件能力注册中心。
     * 返回值可供宿主界面读取插件注册的窗口、命令、服务等能力。
     */
    CapabilityRegistry *capabilityRegistry() const;

    /*
     * 获取插件配置服务。
     * 返回值可供宿主读取插件私有配置的保存位置。
     */
    PluginSettings *pluginSettings() const;

    /*
     * 获取框架日志服务。
     * 返回值可供宿主写入日志或连接日志界面。
     */
    LogService *logService() const;

    /*
     * 获取当前插件记录快照。
     * 返回值包含插件文件路径、名称、状态和加载错误信息。
     */
    QList<PluginRecord> pluginRecords() const;

    /*
     * 查找指定接口类型的插件实例。
     * 返回所有可 qobject_cast 为 T 类型的已加载插件。
     */
    template<typename T>
    QList<T *> findPlugins() const
    {
        QList<T *> result;
        for (const PluginRecord &record : m_records) {
            if (record.state != PluginState::Started || !record.plugin) {
                continue;
            }

            if (T *typedPlugin = qobject_cast<T *>(record.plugin)) {
                result.append(typedPlugin);
            }
        }
        return result;
    }

    /*
     * 添加测试用插件实例。
     * 该函数仅用于单元测试验证插件查询逻辑。
     */
    void addPluginForTest(IPlugin *plugin);

private:
    /*
     * 停止指定插件记录。
     * record 为需要停止的插件记录，只有 Started 状态会触发 stop()。
     */
    void stopPlugin(PluginRecord &record);

    /*
     * 启动所有已扫描且可用的插件。
     * 按元数据依赖关系和服务优先规则决定初始化顺序。
     */
    void startScannedPlugins();

    /*
     * 启动指定插件记录。
     * index 为 m_records 中的插件索引，启动成功时返回 true。
     */
    bool startPlugin(int index);

    /*
     * 判断插件依赖是否已满足。
     * record 为待启动插件，startedPluginIds 为已经启动的插件 id 集合。
     */
    bool dependenciesSatisfied(const PluginRecord &record, const QSet<QString> &startedPluginIds) const;

    /*
     * 标记当前无法满足依赖的插件为失败。
     * pendingIndexes 为尚未启动的插件索引集合。
     */
    void failUnresolvedDependencies(const QSet<int> &pendingIndexes);

    /*
     * 插件管理器持有的共享消息总线。
     */
    MessageBus *m_messageBus = nullptr;
    /*
     * 传递给插件的共享运行上下文。
     */
    PluginContext *m_context = nullptr;
    /*
     * 从应用配置读取的插件启用状态覆盖表。
     */
    QHash<QString, bool> m_enabledOverrides;
    /*
     * 当前扫描和加载到的插件记录列表。
     */
    QList<PluginRecord> m_records;
};

#endif // PLUGINMANAGER_H
