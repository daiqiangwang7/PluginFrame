#include "LogService.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

QString logLevelToString(LogLevel level)
{
    switch (level) {
    case LogLevel::Debug:
        return QStringLiteral("调试");
    case LogLevel::Info:
        return QStringLiteral("信息");
    case LogLevel::Warning:
        return QStringLiteral("警告");
    case LogLevel::Error:
        return QStringLiteral("错误");
    }

    return QStringLiteral("未知");
}

LogService::LogService(QObject *parent)
    : QObject(parent)
{
}

void LogService::setLogFilePath(const QString &filePath)
{
    m_logFilePath = filePath;
}

QString LogService::logFilePath() const
{
    return m_logFilePath;
}

void LogService::log(LogLevel level,
                     const QString &source,
                     const QString &message,
                     const QVariantMap &context)
{
    LogRecord record;
    record.timestamp = QDateTime::currentDateTime();
    record.level = level;
    record.source = source;
    record.message = message;
    record.context = context;

    m_records.append(record);
    appendToFile(record);
    emit recordAdded(record);
}

void LogService::debug(const QString &source, const QString &message, const QVariantMap &context)
{
    log(LogLevel::Debug, source, message, context);
}

void LogService::info(const QString &source, const QString &message, const QVariantMap &context)
{
    log(LogLevel::Info, source, message, context);
}

void LogService::warning(const QString &source, const QString &message, const QVariantMap &context)
{
    log(LogLevel::Warning, source, message, context);
}

void LogService::error(const QString &source, const QString &message, const QVariantMap &context)
{
    log(LogLevel::Error, source, message, context);
}

QList<LogRecord> LogService::records() const
{
    return m_records;
}

void LogService::appendToFile(const LogRecord &record)
{
    if (m_logFilePath.trimmed().isEmpty()) {
        return;
    }

    QDir().mkpath(QFileInfo(m_logFilePath).absolutePath());
    QFile file(m_logFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        return;
    }

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream << record.timestamp.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz"))
           << QStringLiteral(" [") << logLevelToString(record.level) << QStringLiteral("] ")
           << record.source << QStringLiteral(" - ")
           << record.message;
    const QString contextText = contextToString(record.context);
    if (!contextText.isEmpty()) {
        stream << QStringLiteral(" | ") << contextText;
    }
    stream << Qt::endl;
}

QString LogService::contextToString(const QVariantMap &context) const
{
    QStringList parts;
    for (auto it = context.cbegin(); it != context.cend(); ++it) {
        parts.append(QStringLiteral("%1=%2").arg(it.key(), it.value().toString()));
    }
    return parts.join(QStringLiteral("; "));
}
