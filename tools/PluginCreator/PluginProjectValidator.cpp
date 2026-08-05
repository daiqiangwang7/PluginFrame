#include "PluginProjectValidator.h"

#include <QDir>
#include <QRegularExpression>

bool PluginProjectValidator::validate(const PluginProjectOptions &options, QString *errorString)
{
    if (!isValidClassName(options.className.trimmed())) {
        if (errorString) {
            *errorString = QStringLiteral("插件类名必须是合法 C++ 类名，例如 MyViewPlugin。");
        }
        return false;
    }

    if (options.pluginId.trimmed().isEmpty()) {
        if (errorString) {
            *errorString = QStringLiteral("插件 ID 不能为空。");
        }
        return false;
    }

    if (options.displayName.trimmed().isEmpty()) {
        if (errorString) {
            *errorString = QStringLiteral("显示名称不能为空。");
        }
        return false;
    }

    if (options.version.trimmed().isEmpty()) {
        if (errorString) {
            *errorString = QStringLiteral("版本号不能为空。");
        }
        return false;
    }

    if (options.outputDirectory.trimmed().isEmpty()) {
        if (errorString) {
            *errorString = QStringLiteral("输出目录不能为空。");
        }
        return false;
    }

    const QDir outputDir(options.outputDirectory);
    if (outputDir.exists()) {
        const QStringList entries = outputDir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
        if (!entries.isEmpty()) {
            if (errorString) {
                *errorString = QStringLiteral("输出目录已存在且不是空目录，请选择新的目录。");
            }
            return false;
        }
    }

    return true;
}

bool PluginProjectValidator::isValidClassName(const QString &className)
{
    static const QRegularExpression pattern(QStringLiteral("^[A-Za-z_][A-Za-z0-9_]*$"));
    return pattern.match(className).hasMatch();
}
