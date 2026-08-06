#ifndef PLUGINPROJECTVALIDATOR_H
#define PLUGINPROJECTVALIDATOR_H

#include "PluginProjectOptions.h"

class PluginProjectValidator
{
public:
    /*
     * 校验插件创建参数。
     * options 为用户填写的插件信息，errorString 用于返回中文错误原因。
     */
    static bool validate(const PluginProjectOptions &options, QString *errorString);

private:
    /*
     * 判断字符串是否为合法 C++ 类名。
     * className 为待检查文本，合法时返回 true。
     */
    static bool isValidClassName(const QString &className);
};

#endif // PLUGINPROJECTVALIDATOR_H
