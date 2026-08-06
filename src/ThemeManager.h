#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>

class QApplication;

class ThemeManager : public QObject
{
    Q_OBJECT

public:
    /*
     * 创建主题管理器。
     * parent 为 Qt 对象父节点，用于纳入 Qt 对象生命周期管理。
     */
    explicit ThemeManager(QObject *parent = nullptr);

    /*
     * 获取当前主题名称。
     * 返回值为当前已选择的主题标识。
     */
    QString currentTheme() const;

    /*
     * 获取支持的主题名称列表。
     * 返回值包含框架内置的深色和浅色赛博朋克主题。
     */
    QStringList availableThemes() const;

    /*
     * 获取主题文件路径。
     * themeName 为主题名称，返回对应 QSS 文件路径，不存在时返回空字符串。
     */
    QString themeFilePath(const QString &themeName) const;

    /*
     * 应用指定主题。
     * app 为 QApplication 实例，themeName 为主题名称，成功加载并应用时返回 true。
     */
    bool applyTheme(QApplication *app, const QString &themeName);

    /*
     * 切换到下一套主题。
     * app 为 QApplication 实例，返回切换并应用后的主题名称。
     */
    QString toggleTheme(QApplication *app);

private:
    /*
     * 当前已应用的主题名称。
     */
    QString m_currentTheme = QStringLiteral("cyber-dark");
};

#endif // THEMEMANAGER_H
