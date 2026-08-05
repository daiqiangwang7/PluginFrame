#ifndef TEMPLATERENDERER_H
#define TEMPLATERENDERER_H

#include <QMap>
#include <QString>

class TemplateRenderer
{
public:
    /*
     * 渲染文本模板。
     * templateText 为模板内容，variables 为变量表，errorString 用于返回缺失变量错误。
     */
    static QString render(const QString &templateText,
                          const QMap<QString, QString> &variables,
                          QString *errorString = nullptr);
};

#endif // TEMPLATERENDERER_H
