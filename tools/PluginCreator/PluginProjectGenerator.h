#ifndef PLUGINPROJECTGENERATOR_H
#define PLUGINPROJECTGENERATOR_H

#include "PluginProjectOptions.h"

#include <QMap>
#include <QString>

class PluginProjectGenerator
{
public:
    /*
     * 生成插件工程。
     * options 为插件创建参数，返回值包含生成状态、错误信息和文件列表。
     */
    PluginGenerationResult generate(const PluginProjectOptions &options) const;

private:
    /*
     * 生成视图插件工程文件。
     * options 为插件创建参数，result 用于记录生成文件和错误信息。
     */
    bool generateViewPlugin(const PluginProjectOptions &options, PluginGenerationResult *result) const;

    /*
     * 生成服务插件工程文件。
     * options 为插件创建参数，result 用于记录生成文件和错误信息。
     */
    bool generateServicePlugin(const PluginProjectOptions &options, PluginGenerationResult *result) const;

    /*
     * 写入文本文件。
     * filePath 为目标文件路径，content 为文件内容，result 用于记录生成结果。
     */
    bool writeTextFile(const QString &filePath, const QString &content, PluginGenerationResult *result) const;

    /*
     * 渲染模板并写入目标文件。
     * filePath 为目标文件路径，templateText 为模板内容，variables 为模板变量表。
     */
    bool renderToFile(const QString &filePath,
                      const QString &templateText,
                      const QMap<QString, QString> &variables,
                      PluginGenerationResult *result) const;

    /*
     * 将插件目录添加到主工程 CMakeLists.txt。
     * options 为插件创建参数，result 用于记录写入结果和错误信息。
     */
    bool addPluginToRootCMake(const PluginProjectOptions &options, PluginGenerationResult *result) const;

    /*
     * 计算插件目录相对主工程根目录的路径。
     * options 为插件创建参数，返回值用于生成 add_subdirectory 语句。
     */
    QString pluginRelativePath(const PluginProjectOptions &options) const;

    /*
     * 创建模板变量表。
     * options 为插件创建参数，返回值用于模板渲染。
     */
    QMap<QString, QString> variablesForOptions(const PluginProjectOptions &options) const;

    /*
     * 生成插件元数据 JSON 文本。
     * options 为插件创建参数，返回值用于写入 metadata.json。
     */
    QString metadataText(const PluginProjectOptions &options) const;
};

#endif // PLUGINPROJECTGENERATOR_H
