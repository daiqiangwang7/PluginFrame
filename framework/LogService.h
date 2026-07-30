#ifndef LOGSERVICE_H
#define LOGSERVICE_H

#include <QDateTime>
#include <QList>
#include <QObject>
#include <QString>
#include <QVariantMap>

/*
 * 表示日志等级。
 * 用于区分调试、普通信息、警告和错误日志。
 */
enum class LogLevel
{
    Debug,
    Info,
    Warning,
    Error
};

/*
 * 表示单条日志记录。
 * 包含时间、等级、来源、消息和可选上下文数据。
 */
struct LogRecord
{
    QDateTime timestamp;
    LogLevel level = LogLevel::Info;
    QString source;
    QString message;
    QVariantMap context;
};

/*
 * 将日志等级转换为中文显示文本。
 * level 为日志等级，返回值用于界面和文件输出。
 */
QString logLevelToString(LogLevel level);

class LogService : public QObject
{
    Q_OBJECT

public:
    /*
     * 创建日志服务。
     * parent 为 Qt 对象父节点，用于纳入 Qt 对象生命周期管理。
     */
    explicit LogService(QObject *parent = nullptr);

    /*
     * 设置日志文件路径。
     * filePath 为日志落盘路径，设置后后续日志会追加写入该文件。
     */
    void setLogFilePath(const QString &filePath);

    /*
     * 获取日志文件路径。
     * 返回当前日志落盘文件路径。
     */
    QString logFilePath() const;

    /*
     * 写入一条日志。
     * level 为日志等级，source 为日志来源，message 为日志内容，context 为可选上下文。
     */
    void log(LogLevel level,
             const QString &source,
             const QString &message,
             const QVariantMap &context = QVariantMap());

    /*
     * 写入调试日志。
     * source 为日志来源，message 为日志内容，context 为可选上下文。
     */
    void debug(const QString &source, const QString &message, const QVariantMap &context = QVariantMap());

    /*
     * 写入普通信息日志。
     * source 为日志来源，message 为日志内容，context 为可选上下文。
     */
    void info(const QString &source, const QString &message, const QVariantMap &context = QVariantMap());

    /*
     * 写入警告日志。
     * source 为日志来源，message 为日志内容，context 为可选上下文。
     */
    void warning(const QString &source, const QString &message, const QVariantMap &context = QVariantMap());

    /*
     * 写入错误日志。
     * source 为日志来源，message 为日志内容，context 为可选上下文。
     */
    void error(const QString &source, const QString &message, const QVariantMap &context = QVariantMap());

    /*
     * 获取当前内存日志记录。
     * 返回值为日志快照，用于日志界面初始化显示。
     */
    QList<LogRecord> records() const;

signals:
    /*
     * 新日志记录已写入。
     * record 为刚刚写入的日志记录。
     */
    void recordAdded(const LogRecord &record);

private:
    /*
     * 将日志记录追加到文件。
     * record 为需要落盘的日志记录。
     */
    void appendToFile(const LogRecord &record);

    /*
     * 将上下文数据转换为单行文本。
     * context 为日志上下文键值表，返回值用于文件输出。
     */
    QString contextToString(const QVariantMap &context) const;

    QString m_logFilePath;
    QList<LogRecord> m_records;
};

#endif // LOGSERVICE_H
