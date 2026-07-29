#include "PluginMetadata.h"

namespace {

bool readRequiredString(const QJsonObject &json,
                        const QString &key,
                        QString *value,
                        QString *errorString)
{
    if (!json.contains(key) || !json.value(key).isString()
            || json.value(key).toString().trimmed().isEmpty()) {
        if (errorString) {
            *errorString = QStringLiteral("Plugin metadata missing required string field: %1").arg(key);
        }
        return false;
    }

    *value = json.value(key).toString().trimmed();
    return true;
}

} // namespace

PluginMetadata PluginMetadata::fromJsonObject(const QJsonObject &json, QString *errorString)
{
    if (errorString) {
        errorString->clear();
    }

    PluginMetadata metadata;
    if (!readRequiredString(json, QStringLiteral("id"), &metadata.id, errorString)
            || !readRequiredString(json, QStringLiteral("name"), &metadata.name, errorString)
            || !readRequiredString(json, QStringLiteral("displayName"), &metadata.displayName, errorString)
            || !readRequiredString(json, QStringLiteral("version"), &metadata.version, errorString)
            || !readRequiredString(json, QStringLiteral("type"), &metadata.type, errorString)) {
        return PluginMetadata();
    }

    if (metadata.type != QStringLiteral("view") && metadata.type != QStringLiteral("service")) {
        if (errorString) {
            *errorString = QStringLiteral("Plugin metadata field type must be view or service");
        }
        return PluginMetadata();
    }

    if (json.contains(QStringLiteral("enabled"))) {
        if (!json.value(QStringLiteral("enabled")).isBool()) {
            if (errorString) {
                *errorString = QStringLiteral("Plugin metadata field enabled must be bool");
            }
            return PluginMetadata();
        }
        metadata.enabled = json.value(QStringLiteral("enabled")).toBool();
    }

    return metadata;
}

bool PluginMetadata::isValid() const
{
    return !id.isEmpty()
            && !name.isEmpty()
            && !displayName.isEmpty()
            && !version.isEmpty()
            && (type == QStringLiteral("view") || type == QStringLiteral("service"));
}
