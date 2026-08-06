#include "PluginProjectGenerator.h"
#include "PluginProjectValidator.h"
#include "TemplateRenderer.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegExp>
#include <QTextStream>

namespace {

QString upperGuard(const QString &className)
{
    QString guard = className.toUpper();
    guard.replace(QRegExp(QStringLiteral("[^A-Z0-9_]")), QStringLiteral("_"));
    return guard;
}

QString viewPluginHeaderTemplate()
{
    return QStringLiteral(R"TEMPLATE(#ifndef ${HeaderGuard}
#define ${HeaderGuard}

#include "IViewPlugin.h"

class IPluginContext;
class ${WidgetClass};

class ${PluginClass} : public IViewPlugin
{
    Q_OBJECT
    Q_INTERFACES(IPlugin IViewPlugin)
    Q_PLUGIN_METADATA(IID IViewPlugin_iid FILE "metadata.json")

public:
    /*
     * 创建 ${DisplayName} 视图插件对象。
     * parent 为 Qt 对象父节点，可为空。
     */
    explicit ${PluginClass}(QObject *parent = nullptr);

    /*
     * 析构 ${DisplayName} 视图插件对象。
     * 析构时释放插件创建的界面部件。
     */
    ~${PluginClass}() override;

    /*
     * 注入插件运行上下文。
     * context 用于访问消息总线、能力注册、插件配置和日志服务。
     */
    void setContext(IPluginContext *context) override;

    /*
     * 初始化视图插件。
     * 创建插件主界面并注册插件能力，成功时返回 true。
     */
    bool initialize() override;

    /*
     * 启动视图插件。
     * 在插件初始化成功后由宿主调用，可在此订阅消息或启动界面相关逻辑。
     */
    void start() override;

    /*
     * 停止视图插件。
     * 注销插件能力并释放插件主界面。
     */
    void stop() override;

    /*
     * 获取插件内部名称。
     * 返回值用于日志、调试和插件管理。
     */
    QString name() const override;

    /*
     * 获取插件主界面部件。
     * 返回值由宿主窗口管理器承载显示。
     */
    QWidget *widget() override;

    /*
     * 获取插件显示名称。
     * 返回值用于宿主窗口标题或标签文本。
     */
    QString displayName() const override;

private:
    IPluginContext *m_context = nullptr;
    ${WidgetClass} *m_widget = nullptr;
};

#endif // ${HeaderGuard}
)TEMPLATE");
}

QString viewPluginSourceTemplate()
{
    return QStringLiteral(R"TEMPLATE(#include "${PluginClass}.h"
#include "${WidgetHeader}"

#include "CapabilityRegistry.h"
#include "IPluginContext.h"
#include "LogService.h"
#include "MessageBus.h"
#include "PluginSettings.h"

${PluginClass}::${PluginClass}(QObject *parent)
    : IViewPlugin(parent)
{
}

${PluginClass}::~${PluginClass}()
{
    stop();
}

void ${PluginClass}::setContext(IPluginContext *context)
{
    m_context = context;
}

bool ${PluginClass}::initialize()
{
    if (!m_widget) {
        m_widget = new ${WidgetClass};
    }

${ViewCapabilityRegisterBlock}
${ViewSettingsInitBlock}
${ViewLogInitBlock}

    return true;
}

void ${PluginClass}::start()
{
${ViewMessageStartBlock}
${ViewLogStartBlock}
}

void ${PluginClass}::stop()
{
${CapabilityUnregisterBlock}
${ViewLogStopBlock}

    delete m_widget;
    m_widget = nullptr;
}

QString ${PluginClass}::name() const
{
    return QStringLiteral("${PluginClass}");
}

QWidget *${PluginClass}::widget()
{
    return m_widget;
}

QString ${PluginClass}::displayName() const
{
    return QStringLiteral("${DisplayName}");
}
)TEMPLATE");
}

QString widgetHeaderTemplate()
{
    return QStringLiteral(R"TEMPLATE(#ifndef ${WidgetHeaderGuard}
#define ${WidgetHeaderGuard}

#include <QWidget>

namespace Ui {
class ${WidgetClass};
}

class ${WidgetClass} : public QWidget
{
    Q_OBJECT

public:
    /*
     * 创建插件视图部件。
     * parent 为 Qt 对象父节点，可为空。
     */
    explicit ${WidgetClass}(QWidget *parent = nullptr);

    /*
     * 析构插件视图部件。
     * 析构时释放 Qt Designer 生成的界面对象。
     */
    ~${WidgetClass}() override;

private:
    Ui::${WidgetClass} *ui = nullptr;
};

#endif // ${WidgetHeaderGuard}
)TEMPLATE");
}

QString widgetSourceTemplate()
{
    return QStringLiteral(R"TEMPLATE(#include "${WidgetHeader}"
#include "${WidgetUiHeader}"

${WidgetClass}::${WidgetClass}(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::${WidgetClass})
{
    ui->setupUi(this);
}

${WidgetClass}::~${WidgetClass}()
{
    delete ui;
}
)TEMPLATE");
}

QString widgetUiTemplate()
{
    return QStringLiteral(R"TEMPLATE(<?xml version="1.0" encoding="UTF-8"?>
<ui version="4.0">
 <class>${WidgetClass}</class>
 <widget class="QWidget" name="${WidgetClass}">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>600</width>
    <height>400</height>
   </rect>
  </property>
  <property name="windowTitle">
   <string>${DisplayName}</string>
  </property>
  <layout class="QVBoxLayout" name="verticalLayout">
   <item>
    <spacer name="verticalSpacerTop">
     <property name="orientation">
      <enum>Qt::Vertical</enum>
     </property>
     <property name="sizeHint" stdset="0">
      <size>
       <width>20</width>
       <height>40</height>
      </size>
     </property>
    </spacer>
   </item>
   <item>
    <widget class="QLabel" name="titleLabel">
     <property name="alignment">
      <set>Qt::AlignCenter</set>
     </property>
     <property name="text">
      <string>${DisplayName}</string>
     </property>
    </widget>
   </item>
   <item>
    <spacer name="verticalSpacerBottom">
     <property name="orientation">
      <enum>Qt::Vertical</enum>
     </property>
     <property name="sizeHint" stdset="0">
      <size>
       <width>20</width>
       <height>40</height>
      </size>
     </property>
    </spacer>
   </item>
  </layout>
 </widget>
 <resources/>
 <connections/>
</ui>
)TEMPLATE");
}

QString viewCMakeTemplate()
{
    return QStringLiteral(R"TEMPLATE(add_library(${PluginClass} MODULE
    ${PluginClass}.cpp
    ${PluginClass}.h
    ${WidgetClass}.cpp
    ${WidgetClass}.h
    ${WidgetClass}.ui
    metadata.json
)

target_link_libraries(${PluginClass}
    PRIVATE
        Qt5::Core
        Qt5::Widgets
        framework
)

set_target_properties(${PluginClass} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/plugins
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/plugins
)

add_custom_command(TARGET ${PluginClass} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:PluginHost>/plugins
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:${PluginClass}>
            $<TARGET_FILE_DIR:PluginHost>/plugins/$<TARGET_FILE_NAME:${PluginClass}>
)
)TEMPLATE");
}

QString servicePluginHeaderTemplate()
{
    return QStringLiteral(R"TEMPLATE(#ifndef ${HeaderGuard}
#define ${HeaderGuard}

#include "IServicePlugin.h"

class IPluginContext;
class QTimer;

class ${PluginClass} : public IServicePlugin
{
    Q_OBJECT
    Q_INTERFACES(IPlugin IServicePlugin)
    Q_PLUGIN_METADATA(IID IServicePlugin_iid FILE "metadata.json")

public:
    /*
     * 创建 ${DisplayName} 服务插件对象。
     * parent 为 Qt 对象父节点，可为空。
     */
    explicit ${PluginClass}(QObject *parent = nullptr);

    /*
     * 析构 ${DisplayName} 服务插件对象。
     * 析构时停止后台服务并释放资源。
     */
    ~${PluginClass}() override;

    /*
     * 注入插件运行上下文。
     * context 用于访问消息总线、能力注册、插件配置和日志服务。
     */
    void setContext(IPluginContext *context) override;

    /*
     * 初始化服务插件。
     * 创建后台定时器或业务对象，成功时返回 true。
     */
    bool initialize() override;

    /*
     * 启动服务插件。
     * 在此函数中启动后台定时器、线程或业务服务。
     */
    void start() override;

    /*
     * 停止服务插件。
     * 停止后台任务并释放运行期资源。
     */
    void stop() override;

    /*
     * 获取插件内部名称。
     * 返回值用于日志、调试和插件管理。
     */
    QString name() const override;

private:
    /*
     * 发布服务心跳消息。
     * 消息主题为插件 ID 加 `.heartbeat` 后缀。
     */
    void publishHeartbeat();

    IPluginContext *m_context = nullptr;
    QTimer *m_timer = nullptr;
};

#endif // ${HeaderGuard}
)TEMPLATE");
}

QString servicePluginSourceTemplate()
{
    return QStringLiteral(R"TEMPLATE(#include "${PluginClass}.h"

#include "CapabilityRegistry.h"
#include "IPluginContext.h"
#include "LogService.h"
#include "MessageBus.h"
#include "PluginSettings.h"

#include <QDateTime>
#include <QTimer>

${PluginClass}::${PluginClass}(QObject *parent)
    : IServicePlugin(parent)
{
}

${PluginClass}::~${PluginClass}()
{
    stop();
}

void ${PluginClass}::setContext(IPluginContext *context)
{
    m_context = context;
}

bool ${PluginClass}::initialize()
{
    if (!m_timer) {
        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &${PluginClass}::publishHeartbeat);
    }

${ServiceCapabilityRegisterBlock}
${ServiceSettingsInitBlock}
${ServiceLogInitBlock}

    return true;
}

void ${PluginClass}::start()
{
    if (m_timer && !m_timer->isActive()) {
        m_timer->start(1000);
    }
    publishHeartbeat();

${ServiceLogStartBlock}
}

void ${PluginClass}::stop()
{
    if (m_timer) {
        m_timer->stop();
    }

${CapabilityUnregisterBlock}
${ServiceLogStopBlock}
}

QString ${PluginClass}::name() const
{
    return QStringLiteral("${PluginClass}");
}

void ${PluginClass}::publishHeartbeat()
{
${ServiceHeartbeatBlock}
}
)TEMPLATE");
}

QString serviceCMakeTemplate()
{
    return QStringLiteral(R"TEMPLATE(add_library(${PluginClass} MODULE
    ${PluginClass}.cpp
    ${PluginClass}.h
    metadata.json
)

target_link_libraries(${PluginClass}
    PRIVATE
        Qt5::Core
        Qt5::Widgets
        framework
)

set_target_properties(${PluginClass} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/plugins
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/plugins
)

add_custom_command(TARGET ${PluginClass} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:PluginHost>/plugins
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:${PluginClass}>
            $<TARGET_FILE_DIR:PluginHost>/plugins/$<TARGET_FILE_NAME:${PluginClass}>
)
)TEMPLATE");
}

} // namespace

QString creatorPluginTypeToMetadataType(CreatorPluginType type)
{
    return type == CreatorPluginType::View ? QStringLiteral("view") : QStringLiteral("service");
}

QString creatorWindowAreaToConfigText(CreatorWindowArea area)
{
    switch (area) {
    case CreatorWindowArea::LeftDock:
        return QStringLiteral("leftDock");
    case CreatorWindowArea::RightDock:
        return QStringLiteral("rightDock");
    case CreatorWindowArea::BottomDock:
        return QStringLiteral("bottomDock");
    case CreatorWindowArea::Floating:
        return QStringLiteral("floating");
    case CreatorWindowArea::Central:
    default:
        return QStringLiteral("central");
    }
}

PluginGenerationResult PluginProjectGenerator::generate(const PluginProjectOptions &options) const
{
    PluginGenerationResult result;
    result.outputDirectory = options.outputDirectory;

    QString errorString;
    if (!PluginProjectValidator::validate(options, &errorString)) {
        result.message = errorString;
        return result;
    }

    QDir dir;
    if (!dir.mkpath(options.outputDirectory)) {
        result.message = QStringLiteral("创建输出目录失败：%1").arg(options.outputDirectory);
        return result;
    }

    const bool ok = options.type == CreatorPluginType::View
            ? generateViewPlugin(options, &result)
            : generateServicePlugin(options, &result);

    if (!ok) {
        return result;
    }

    if (options.autoAddToProject && !addPluginToRootCMake(options, &result)) {
        return result;
    }

    result.success = true;
    result.message = options.autoAddToProject
            ? QStringLiteral("插件工程生成成功，并已添加到主工程。")
            : QStringLiteral("插件工程生成成功。");
    return result;
}

bool PluginProjectGenerator::generateViewPlugin(const PluginProjectOptions &options, PluginGenerationResult *result) const
{
    const QMap<QString, QString> variables = variablesForOptions(options);
    const QDir dir(options.outputDirectory);
    const QString pluginClass = variables.value(QStringLiteral("PluginClass"));
    const QString widgetClass = variables.value(QStringLiteral("WidgetClass"));

    if (!renderToFile(dir.filePath(pluginClass + QStringLiteral(".h")), viewPluginHeaderTemplate(), variables, result)) {
        return false;
    }
    if (!renderToFile(dir.filePath(pluginClass + QStringLiteral(".cpp")), viewPluginSourceTemplate(), variables, result)) {
        return false;
    }
    if (!renderToFile(dir.filePath(widgetClass + QStringLiteral(".h")), widgetHeaderTemplate(), variables, result)) {
        return false;
    }
    if (!renderToFile(dir.filePath(widgetClass + QStringLiteral(".cpp")), widgetSourceTemplate(), variables, result)) {
        return false;
    }
    if (!renderToFile(dir.filePath(widgetClass + QStringLiteral(".ui")), widgetUiTemplate(), variables, result)) {
        return false;
    }
    if (!renderToFile(dir.filePath(QStringLiteral("CMakeLists.txt")), viewCMakeTemplate(), variables, result)) {
        return false;
    }
    return writeTextFile(dir.filePath(QStringLiteral("metadata.json")), metadataText(options), result);
}

bool PluginProjectGenerator::generateServicePlugin(const PluginProjectOptions &options, PluginGenerationResult *result) const
{
    const QMap<QString, QString> variables = variablesForOptions(options);
    const QDir dir(options.outputDirectory);
    const QString pluginClass = variables.value(QStringLiteral("PluginClass"));

    if (!renderToFile(dir.filePath(pluginClass + QStringLiteral(".h")), servicePluginHeaderTemplate(), variables, result)) {
        return false;
    }
    if (!renderToFile(dir.filePath(pluginClass + QStringLiteral(".cpp")), servicePluginSourceTemplate(), variables, result)) {
        return false;
    }
    if (!renderToFile(dir.filePath(QStringLiteral("CMakeLists.txt")), serviceCMakeTemplate(), variables, result)) {
        return false;
    }
    return writeTextFile(dir.filePath(QStringLiteral("metadata.json")), metadataText(options), result);
}

bool PluginProjectGenerator::writeTextFile(const QString &filePath,
                                           const QString &content,
                                           PluginGenerationResult *result) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (result) {
            result->message = QStringLiteral("写入文件失败：%1").arg(filePath);
        }
        return false;
    }

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream << content;

    if (result) {
        result->generatedFiles.append(filePath);
    }
    return true;
}

bool PluginProjectGenerator::renderToFile(const QString &filePath,
                                          const QString &templateText,
                                          const QMap<QString, QString> &variables,
                                          PluginGenerationResult *result) const
{
    QString errorString;
    const QString content = TemplateRenderer::render(templateText, variables, &errorString);
    if (!errorString.isEmpty()) {
        if (result) {
            result->message = errorString;
        }
        return false;
    }

    return writeTextFile(filePath, content, result);
}

bool PluginProjectGenerator::addPluginToRootCMake(const PluginProjectOptions &options,
                                                  PluginGenerationResult *result) const
{
    const QString projectRoot = options.projectRootDirectory.trimmed();
    if (projectRoot.isEmpty()) {
        if (result) {
            result->message = QStringLiteral("自动加入主工程失败：项目根目录为空。");
        }
        return false;
    }

    const QString cmakePath = QDir(projectRoot).filePath(QStringLiteral("CMakeLists.txt"));
    QFile file(cmakePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (result) {
            result->message = QStringLiteral("自动加入主工程失败：无法读取 %1。").arg(cmakePath);
        }
        return false;
    }

    QTextStream input(&file);
    input.setCodec("UTF-8");
    QString content = input.readAll();
    file.close();

    const QString relativePath = pluginRelativePath(options);
    const QString addLine = QStringLiteral("add_subdirectory(%1)").arg(relativePath);
    if (content.contains(addLine)) {
        if (result) {
            result->generatedFiles.append(cmakePath + QStringLiteral("（已存在 add_subdirectory，未重复写入）"));
        }
        return true;
    }

    if (!content.endsWith(QLatin1Char('\n'))) {
        content.append(QLatin1Char('\n'));
    }
    content.append(addLine);
    content.append(QLatin1Char('\n'));

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        if (result) {
            result->message = QStringLiteral("自动加入主工程失败：无法写入 %1。").arg(cmakePath);
        }
        return false;
    }

    QTextStream output(&file);
    output.setCodec("UTF-8");
    output << content;

    if (result) {
        result->generatedFiles.append(cmakePath);
    }
    return true;
}

QString PluginProjectGenerator::pluginRelativePath(const PluginProjectOptions &options) const
{
    const QDir projectRoot(options.projectRootDirectory);
    QString relativePath = projectRoot.relativeFilePath(options.outputDirectory);
    relativePath = QDir::fromNativeSeparators(relativePath);
    if (relativePath.startsWith(QStringLiteral("../"))) {
        relativePath = QStringLiteral("plugins/%1").arg(options.className.trimmed());
    }
    return relativePath;
}

QMap<QString, QString> PluginProjectGenerator::variablesForOptions(const PluginProjectOptions &options) const
{
    const QString className = options.className.trimmed();
    const QString widgetClass = className + QStringLiteral("Widget");
    const QString pluginId = options.pluginId.trimmed();
    const QString displayName = options.displayName.trimmed();

    QMap<QString, QString> variables;
    variables.insert(QStringLiteral("PluginClass"), className);
    variables.insert(QStringLiteral("PluginId"), pluginId);
    variables.insert(QStringLiteral("DisplayName"), displayName);
    variables.insert(QStringLiteral("Version"), options.version.trimmed());
    variables.insert(QStringLiteral("Author"), options.author.trimmed());
    variables.insert(QStringLiteral("Description"), options.description.trimmed());
    variables.insert(QStringLiteral("HeaderGuard"), upperGuard(className) + QStringLiteral("_H"));
    variables.insert(QStringLiteral("WidgetClass"), widgetClass);
    variables.insert(QStringLiteral("WidgetHeader"), widgetClass + QStringLiteral(".h"));
    variables.insert(QStringLiteral("WidgetHeaderGuard"), upperGuard(widgetClass) + QStringLiteral("_H"));
    variables.insert(QStringLiteral("WidgetUiHeader"), QStringLiteral("ui_") + widgetClass + QStringLiteral(".h"));
    variables.insert(QStringLiteral("MetadataType"), creatorPluginTypeToMetadataType(options.type));
    variables.insert(QStringLiteral("WindowArea"), creatorWindowAreaToConfigText(options.area));
    variables.insert(QStringLiteral("CMAKE_BINARY_DIR"), QStringLiteral("${CMAKE_BINARY_DIR}"));
    variables.insert(QStringLiteral("CMAKE_COMMAND"), QStringLiteral("${CMAKE_COMMAND}"));
    variables.insert(QStringLiteral("ViewCapabilityRegisterBlock"), options.registerCapability
                     ? QStringLiteral(R"CODE(    if (m_context && m_context->capabilityRegistry()) {
        m_context->capabilityRegistry()->registerCapability({
            QStringLiteral("%1"),
            QStringLiteral("%1.view"),
            QStringLiteral("view"),
            displayName(),
            name()
        });
    }
)CODE").arg(pluginId)
                     : QString());
    variables.insert(QStringLiteral("ServiceCapabilityRegisterBlock"), options.registerCapability
                     ? QStringLiteral(R"CODE(    if (m_context && m_context->capabilityRegistry()) {
        m_context->capabilityRegistry()->registerCapability({
            QStringLiteral("%1"),
            QStringLiteral("%1.service"),
            QStringLiteral("service"),
            QStringLiteral("%2"),
            name()
        });
    }
)CODE").arg(pluginId, displayName)
                     : QString());
    variables.insert(QStringLiteral("CapabilityUnregisterBlock"), options.registerCapability
                     ? QStringLiteral(R"CODE(    if (m_context && m_context->capabilityRegistry()) {
        m_context->capabilityRegistry()->unregisterPluginCapabilities(QStringLiteral("%1"));
    }
)CODE").arg(pluginId)
                     : QString());
    variables.insert(QStringLiteral("ViewSettingsInitBlock"), options.usePluginSettings
                     ? QStringLiteral(R"CODE(    if (m_context && m_context->pluginSettings()) {
        m_context->pluginSettings()->setValue(QStringLiteral("%1"),
                                              QStringLiteral("ui/displayName"),
                                              displayName());
    }
)CODE").arg(pluginId)
                     : QString());
    variables.insert(QStringLiteral("ServiceSettingsInitBlock"), options.usePluginSettings
                     ? QStringLiteral(R"CODE(    if (m_context && m_context->pluginSettings()) {
        m_context->pluginSettings()->setValue(QStringLiteral("%1"),
                                              QStringLiteral("service/enabled"),
                                              true);
    }
)CODE").arg(pluginId)
                     : QString());
    variables.insert(QStringLiteral("ViewMessageStartBlock"), options.useMessageBus
                     ? QStringLiteral(R"CODE(    if (m_context && m_context->messageBus()) {
        m_context->messageBus()->publish(QStringLiteral("%1.started"),
                                         {{QStringLiteral("plugin"), name()}});
    }
)CODE").arg(pluginId)
                     : QString());
    variables.insert(QStringLiteral("ServiceHeartbeatBlock"), options.useMessageBus
                     ? QStringLiteral(R"CODE(    if (!m_context || !m_context->messageBus()) {
        return;
    }

    m_context->messageBus()->publish(QStringLiteral("%1.heartbeat"),
                                     {{QStringLiteral("plugin"), name()},
                                      {QStringLiteral("time"), QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"))}});
)CODE").arg(pluginId)
                     : QStringLiteral("    Q_UNUSED(this);"));
    variables.insert(QStringLiteral("ViewLogInitBlock"), options.useLogService
                     ? QStringLiteral(R"CODE(    if (m_context && m_context->logService()) {
        m_context->logService()->info(name(), QStringLiteral("视图插件初始化完成"));
    }
)CODE")
                     : QString());
    variables.insert(QStringLiteral("ViewLogStartBlock"), options.useLogService
                     ? QStringLiteral(R"CODE(    if (m_context && m_context->logService()) {
        m_context->logService()->info(name(), QStringLiteral("视图插件已启动"));
    }
)CODE")
                     : QString());
    variables.insert(QStringLiteral("ViewLogStopBlock"), options.useLogService
                     ? QStringLiteral(R"CODE(    if (m_context && m_context->logService()) {
        m_context->logService()->info(name(), QStringLiteral("视图插件已停止"));
    }
)CODE")
                     : QString());
    variables.insert(QStringLiteral("ServiceLogInitBlock"), options.useLogService
                     ? QStringLiteral(R"CODE(    if (m_context && m_context->logService()) {
        m_context->logService()->info(name(), QStringLiteral("服务插件初始化完成"));
    }
)CODE")
                     : QString());
    variables.insert(QStringLiteral("ServiceLogStartBlock"), options.useLogService
                     ? QStringLiteral(R"CODE(    if (m_context && m_context->logService()) {
        m_context->logService()->info(name(), QStringLiteral("服务插件已启动"));
    }
)CODE")
                     : QString());
    variables.insert(QStringLiteral("ServiceLogStopBlock"), options.useLogService
                     ? QStringLiteral(R"CODE(    if (m_context && m_context->logService()) {
        m_context->logService()->info(name(), QStringLiteral("服务插件已停止"));
    }
)CODE")
                     : QString());
    return variables;
}

QString PluginProjectGenerator::metadataText(const PluginProjectOptions &options) const
{
    QJsonObject metadata;
    metadata.insert(QStringLiteral("id"), options.pluginId.trimmed());
    metadata.insert(QStringLiteral("name"), options.className.trimmed());
    metadata.insert(QStringLiteral("displayName"), options.displayName.trimmed());
    metadata.insert(QStringLiteral("version"), options.version.trimmed());
    metadata.insert(QStringLiteral("type"), creatorPluginTypeToMetadataType(options.type));
    metadata.insert(QStringLiteral("enabled"), true);
    metadata.insert(QStringLiteral("author"), options.author.trimmed());
    metadata.insert(QStringLiteral("description"), options.description.trimmed());

    if (options.type == CreatorPluginType::View) {
        metadata.insert(QStringLiteral("area"), creatorWindowAreaToConfigText(options.area));
    }

    return QString::fromUtf8(QJsonDocument(metadata).toJson(QJsonDocument::Indented));
}
