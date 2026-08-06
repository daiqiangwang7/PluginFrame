#ifndef PLUGINPROJECTOPTIONS_H
#define PLUGINPROJECTOPTIONS_H

#include <QString>
#include <QStringList>

enum class CreatorPluginType
{
    /*
     * 创建视图插件。
     */
    View,
    /*
     * 创建服务插件。
     */
    Service
};

enum class CreatorWindowArea
{
    /*
     * 生成的视图插件默认注册到中心区。
     */
    Central,
    /*
     * 生成的视图插件默认注册到左侧停靠区。
     */
    LeftDock,
    /*
     * 生成的视图插件默认注册到右侧停靠区。
     */
    RightDock,
    /*
     * 生成的视图插件默认注册到底部停靠区。
     */
    BottomDock,
    /*
     * 生成的视图插件默认注册为浮动窗口。
     */
    Floating
};

struct PluginProjectOptions
{
    /*
     * 需要创建的插件类型。
     */
    CreatorPluginType type = CreatorPluginType::View;
    /*
     * 视图插件默认窗口区域。
     */
    CreatorWindowArea area = CreatorWindowArea::Central;
    /*
     * 插件 C++ 类名。
     */
    QString className;
    /*
     * 插件唯一标识。
     */
    QString pluginId;
    /*
     * 插件显示名称。
     */
    QString displayName;
    /*
     * 插件版本号。
     */
    QString version = QStringLiteral("1.0.0");
    /*
     * 插件作者信息。
     */
    QString author;
    /*
     * 插件描述信息。
     */
    QString description;
    /*
     * 当前框架项目根目录。
     */
    QString projectRootDirectory;
    /*
     * 插件工程输出目录。
     */
    QString outputDirectory;
    /*
     * 是否自动把插件目录加入主工程 CMakeLists.txt。
     */
    bool autoAddToProject = true;
    /*
     * 生成代码是否默认注册插件能力。
     */
    bool registerCapability = true;
    /*
     * 生成代码是否默认使用消息总线。
     */
    bool useMessageBus = true;
    /*
     * 生成代码是否默认使用日志服务。
     */
    bool useLogService = true;
    /*
     * 生成代码是否默认使用插件私有配置。
     */
    bool usePluginSettings = true;
};

struct PluginGenerationResult
{
    /*
     * 插件工程是否生成成功。
     */
    bool success = false;
    /*
     * 生成结果或错误提示文本。
     */
    QString message;
    /*
     * 实际输出目录。
     */
    QString outputDirectory;
    /*
     * 本次生成或修改的文件列表。
     */
    QStringList generatedFiles;
};

QString creatorPluginTypeToMetadataType(CreatorPluginType type);
QString creatorWindowAreaToConfigText(CreatorWindowArea area);

#endif // PLUGINPROJECTOPTIONS_H
