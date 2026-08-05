#ifndef PLUGINPROJECTOPTIONS_H
#define PLUGINPROJECTOPTIONS_H

#include <QString>
#include <QStringList>

enum class CreatorPluginType
{
    View,
    Service
};

enum class CreatorWindowArea
{
    Central,
    LeftDock,
    RightDock,
    BottomDock,
    Floating
};

struct PluginProjectOptions
{
    CreatorPluginType type = CreatorPluginType::View;
    CreatorWindowArea area = CreatorWindowArea::Central;
    QString className;
    QString pluginId;
    QString displayName;
    QString version = QStringLiteral("1.0.0");
    QString author;
    QString description;
    QString projectRootDirectory;
    QString outputDirectory;
    bool autoAddToProject = true;
    bool registerCapability = true;
    bool useMessageBus = true;
    bool useLogService = true;
    bool usePluginSettings = true;
};

struct PluginGenerationResult
{
    bool success = false;
    QString message;
    QString outputDirectory;
    QStringList generatedFiles;
};

QString creatorPluginTypeToMetadataType(CreatorPluginType type);
QString creatorWindowAreaToConfigText(CreatorWindowArea area);

#endif // PLUGINPROJECTOPTIONS_H
