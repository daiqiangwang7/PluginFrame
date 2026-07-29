#include "ThemeManager.h"

#include <QApplication>
#include <QFile>

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
{
}

QString ThemeManager::currentTheme() const
{
    return m_currentTheme;
}

QStringList ThemeManager::availableThemes() const
{
    return {QStringLiteral("cyber-dark"), QStringLiteral("cyber-light")};
}

QString ThemeManager::themeFilePath(const QString &themeName) const
{
    if (!availableThemes().contains(themeName)) {
        return QString();
    }

    return QApplication::applicationDirPath()
            + QStringLiteral("/themes/")
            + themeName
            + QStringLiteral(".qss");
}

bool ThemeManager::applyTheme(QApplication *app, const QString &themeName)
{
    if (!app) {
        return false;
    }

    const QString filePath = themeFilePath(themeName);
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    app->setStyleSheet(QString::fromUtf8(file.readAll()));
    m_currentTheme = themeName;
    return true;
}

QString ThemeManager::toggleTheme(QApplication *app)
{
    const QString nextTheme = m_currentTheme == QStringLiteral("cyber-dark")
            ? QStringLiteral("cyber-light")
            : QStringLiteral("cyber-dark");
    applyTheme(app, nextTheme);
    return m_currentTheme;
}
