#include "TemplateRenderer.h"

#include <QRegularExpression>

QString TemplateRenderer::render(const QString &templateText,
                                 const QMap<QString, QString> &variables,
                                 QString *errorString)
{
    QString result = templateText;
    static const QRegularExpression pattern(QStringLiteral("\\$\\{([A-Za-z0-9_]+)\\}"));
    QRegularExpressionMatchIterator iterator = pattern.globalMatch(templateText);

    while (iterator.hasNext()) {
        const QRegularExpressionMatch match = iterator.next();
        const QString token = match.captured(0);
        const QString key = match.captured(1);

        if (!variables.contains(key)) {
            if (errorString) {
                *errorString = QStringLiteral("模板变量缺失：%1").arg(key);
            }
            return QString();
        }

        result.replace(token, variables.value(key));
    }

    return result;
}
