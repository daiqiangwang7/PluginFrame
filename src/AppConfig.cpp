#include "AppConfig.h"

#include <QDomDocument>
#include <QFile>

namespace {

QString normalizedAreaText(const QString &text)
{
    return text.trimmed().toLower();
}

} // namespace

AppConfig AppConfig::fromXml(const QByteArray &xml, QString *errorString)
{
    if (errorString) {
        errorString->clear();
    }

    QDomDocument document;
    QString parseError;
    int errorLine = 0;
    int errorColumn = 0;
    if (!document.setContent(xml, &parseError, &errorLine, &errorColumn)) {
        if (errorString) {
            *errorString = QStringLiteral("XML parse error at %1:%2: %3")
                    .arg(errorLine)
                    .arg(errorColumn)
                    .arg(parseError);
        }
        return AppConfig();
    }

    AppConfig config;
    const QDomElement root = document.documentElement();
    const QDomElement themeElement = root.firstChildElement(QStringLiteral("theme"));
    if (!themeElement.isNull() && themeElement.hasAttribute(QStringLiteral("default"))) {
        config.defaultTheme = themeElement.attribute(QStringLiteral("default")).trimmed();
    }

    const QDomElement pluginsElement = root.firstChildElement(QStringLiteral("plugins"));
    QDomElement pluginElement = pluginsElement.firstChildElement(QStringLiteral("plugin"));
    while (!pluginElement.isNull()) {
        const QString id = pluginElement.attribute(QStringLiteral("id")).trimmed();
        if (!id.isEmpty()) {
            PluginConfig pluginConfig;
            if (pluginElement.hasAttribute(QStringLiteral("enabled"))) {
                pluginConfig.enabled = pluginElement.attribute(QStringLiteral("enabled")).trimmed()
                        .compare(QStringLiteral("false"), Qt::CaseInsensitive) != 0;
            }
            if (pluginElement.hasAttribute(QStringLiteral("area"))) {
                pluginConfig.area = windowAreaFromString(pluginElement.attribute(QStringLiteral("area")));
            }
            config.pluginSettings.insert(id, pluginConfig);
        }

        pluginElement = pluginElement.nextSiblingElement(QStringLiteral("plugin"));
    }

    return config;
}

AppConfig AppConfig::fromFile(const QString &filePath, QString *errorString)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorString) {
            *errorString = QStringLiteral("Can not open config file: %1").arg(filePath);
        }
        return AppConfig();
    }

    return fromXml(file.readAll(), errorString);
}

bool AppConfig::isPluginEnabled(const QString &pluginId) const
{
    if (!pluginSettings.contains(pluginId)) {
        return true;
    }

    return pluginSettings.value(pluginId).enabled;
}

WindowArea AppConfig::pluginArea(const QString &pluginId) const
{
    if (!pluginSettings.contains(pluginId)) {
        return WindowArea::Central;
    }

    return pluginSettings.value(pluginId).area;
}

WindowArea windowAreaFromString(const QString &text)
{
    const QString areaText = normalizedAreaText(text);
    if (areaText == QStringLiteral("leftdock")) {
        return WindowArea::LeftDock;
    }
    if (areaText == QStringLiteral("rightdock")) {
        return WindowArea::RightDock;
    }
    if (areaText == QStringLiteral("bottomdock")) {
        return WindowArea::BottomDock;
    }
    if (areaText == QStringLiteral("floating")) {
        return WindowArea::Floating;
    }

    return WindowArea::Central;
}
