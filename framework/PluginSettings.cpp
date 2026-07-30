#include "PluginSettings.h"

#include <QDir>
#include <QSettings>

PluginSettings::PluginSettings(const QString &basePath, QObject *parent)
    : QObject(parent)
    , m_basePath(basePath)
{
}

void PluginSettings::setBasePath(const QString &basePath)
{
    m_basePath = basePath;
}

QString PluginSettings::basePath() const
{
    return m_basePath;
}

QVariant PluginSettings::value(const QString &pluginId, const QString &key, const QVariant &defaultValue) const
{
    QSettings settings(pluginFilePath(pluginId), QSettings::IniFormat);
    return settings.value(key, defaultValue);
}

void PluginSettings::setValue(const QString &pluginId, const QString &key, const QVariant &value)
{
    QDir().mkpath(m_basePath);
    QSettings settings(pluginFilePath(pluginId), QSettings::IniFormat);
    settings.setValue(key, value);
    settings.sync();
}

bool PluginSettings::contains(const QString &pluginId, const QString &key) const
{
    QSettings settings(pluginFilePath(pluginId), QSettings::IniFormat);
    return settings.contains(key);
}

void PluginSettings::remove(const QString &pluginId, const QString &key)
{
    QSettings settings(pluginFilePath(pluginId), QSettings::IniFormat);
    settings.remove(key);
    settings.sync();
}

QString PluginSettings::pluginFilePath(const QString &pluginId) const
{
    return QDir(m_basePath).absoluteFilePath(safePluginFileName(pluginId) + QStringLiteral(".ini"));
}

QString PluginSettings::safePluginFileName(const QString &pluginId) const
{
    QString fileName = pluginId.trimmed();
    for (QChar &character : fileName) {
        if (!character.isLetterOrNumber() && character != QLatin1Char('.') && character != QLatin1Char('-')) {
            character = QLatin1Char('_');
        }
    }
    return fileName.isEmpty() ? QStringLiteral("unknown-plugin") : fileName;
}
