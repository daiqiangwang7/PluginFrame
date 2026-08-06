# PluginCreator Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add a Qt Widgets graphical wizard named `PluginCreator.exe` that generates standard PluginFrame view plugins and service plugins.

**Architecture:** The tool is a separate executable under `tools/PluginCreator`. UI code collects user input, generator code validates options and writes files, and a small template renderer expands `${Variable}` placeholders into concrete plugin source files.

**Tech Stack:** C++11, Qt 5 Core/Widgets, CMake 3.16+, PluginFrame framework headers.

---

## File Structure

- Create: `tools/PluginCreator/CMakeLists.txt`
  - Defines the `PluginCreator` executable and links Qt5 Core/Widgets.
- Create: `tools/PluginCreator/main.cpp`
  - Starts the Qt application and shows the creator window.
- Create: `tools/PluginCreator/PluginProjectOptions.h`
  - Defines plugin type, common options, and generation result structs.
- Create: `tools/PluginCreator/PluginProjectValidator.h`
- Create: `tools/PluginCreator/PluginProjectValidator.cpp`
  - Validates class name, plugin id, display name, and output directory.
- Create: `tools/PluginCreator/TemplateRenderer.h`
- Create: `tools/PluginCreator/TemplateRenderer.cpp`
  - Replaces `${Name}` style variables in template text.
- Create: `tools/PluginCreator/PluginProjectGenerator.h`
- Create: `tools/PluginCreator/PluginProjectGenerator.cpp`
  - Generates view plugin and service plugin project files.
- Create: `tools/PluginCreator/PluginCreatorWindow.h`
- Create: `tools/PluginCreator/PluginCreatorWindow.cpp`
- Create: `tools/PluginCreator/PluginCreatorWindow.ui`
  - Provides a Qt graphical wizard for collecting plugin information.
- Modify: `CMakeLists.txt`
  - Adds `add_subdirectory(tools/PluginCreator)`.
- Modify: `docs/框架使用说明.md`
  - Documents how to use `PluginCreator.exe`.

---

## Task 1: Add PluginCreator Build Target

**Files:**
- Create: `tools/PluginCreator/CMakeLists.txt`
- Create: `tools/PluginCreator/main.cpp`
- Modify: `CMakeLists.txt`

- [ ] **Step 1: Create the tool CMake file**

Create `tools/PluginCreator/CMakeLists.txt`:

```cmake
add_executable(PluginCreator
    main.cpp
    PluginCreatorWindow.cpp
    PluginCreatorWindow.h
    PluginCreatorWindow.ui
    PluginProjectGenerator.cpp
    PluginProjectGenerator.h
    PluginProjectOptions.h
    PluginProjectValidator.cpp
    PluginProjectValidator.h
    TemplateRenderer.cpp
    TemplateRenderer.h
)

target_link_libraries(PluginCreator
    PRIVATE
        Qt5::Core
        Qt5::Widgets
)

set_target_properties(PluginCreator PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
```

- [ ] **Step 2: Create the main entry**

Create `tools/PluginCreator/main.cpp`:

```cpp
#include "PluginCreatorWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    PluginCreatorWindow window;
    window.show();

    return app.exec();
}
```

- [ ] **Step 3: Register the tool in root CMake**

Modify root `CMakeLists.txt` and add this line after `add_subdirectory(src)`:

```cmake
add_subdirectory(tools/PluginCreator)
```

- [ ] **Step 4: Configure build to verify the target exists**

Run:

```powershell
cmake -S . -B build
```

Expected: CMake configure succeeds and generates a `PluginCreator` target.

- [ ] **Step 5: Commit**

Run:

```powershell
git add CMakeLists.txt tools/PluginCreator/CMakeLists.txt tools/PluginCreator/main.cpp
git commit -m "添加插件创建器构建目标"
```

---

## Task 2: Add Generator Data Types

**Files:**
- Create: `tools/PluginCreator/PluginProjectOptions.h`

- [ ] **Step 1: Create option and result types**

Create `tools/PluginCreator/PluginProjectOptions.h`:

```cpp
#ifndef PLUGINPROJECTOPTIONS_H
#define PLUGINPROJECTOPTIONS_H

#include <QMap>
#include <QString>
#include <QStringList>

enum class CreatorPluginType
{
    View,
    Service
};

enum class CreatorWindowArea
{
    Central,
    LeftDock,
    RightDock,
    BottomDock,
    Floating
};

struct PluginProjectOptions
{
    CreatorPluginType type = CreatorPluginType::View;
    CreatorWindowArea area = CreatorWindowArea::Central;
    QString className;
    QString pluginId;
    QString displayName;
    QString version = QStringLiteral("1.0.0");
    QString author;
    QString description;
    QString outputDirectory;
    bool registerCapability = true;
    bool useMessageBus = true;
    bool useLogService = true;
    bool usePluginSettings = true;
};

struct PluginGenerationResult
{
    bool success = false;
    QString message;
    QString outputDirectory;
    QStringList generatedFiles;
};

QString creatorPluginTypeToMetadataType(CreatorPluginType type);
QString creatorWindowAreaToConfigText(CreatorWindowArea area);

#endif // PLUGINPROJECTOPTIONS_H
```

- [ ] **Step 2: Add helper implementation to generator later**

Do not create a separate `.cpp` for this task. Implement `creatorPluginTypeToMetadataType()` and `creatorWindowAreaToConfigText()` in `PluginProjectGenerator.cpp` in Task 5 so the tool stays small.

- [ ] **Step 3: Commit**

Run:

```powershell
git add tools/PluginCreator/PluginProjectOptions.h
git commit -m "添加插件创建器数据结构"
```

---

## Task 3: Add Input Validation

**Files:**
- Create: `tools/PluginCreator/PluginProjectValidator.h`
- Create: `tools/PluginCreator/PluginProjectValidator.cpp`

- [ ] **Step 1: Create validator header with required comments**

Create `tools/PluginCreator/PluginProjectValidator.h`:

```cpp
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
```

- [ ] **Step 2: Create validator implementation**

Create `tools/PluginCreator/PluginProjectValidator.cpp`:

```cpp
#include "PluginProjectValidator.h"

#include <QDir>
#include <QRegularExpression>

bool PluginProjectValidator::validate(const PluginProjectOptions &options, QString *errorString)
{
    if (!isValidClassName(options.className)) {
        if (errorString) {
            *errorString = QStringLiteral("插件类名必须是合法 C++ 类名，例如 MyViewPlugin。");
        }
        return false;
    }

    if (options.pluginId.trimmed().isEmpty()) {
        if (errorString) {
            *errorString = QStringLiteral("插件 ID 不能为空。");
        }
        return false;
    }

    if (options.displayName.trimmed().isEmpty()) {
        if (errorString) {
            *errorString = QStringLiteral("显示名称不能为空。");
        }
        return false;
    }

    if (options.version.trimmed().isEmpty()) {
        if (errorString) {
            *errorString = QStringLiteral("版本号不能为空。");
        }
        return false;
    }

    if (options.outputDirectory.trimmed().isEmpty()) {
        if (errorString) {
            *errorString = QStringLiteral("输出目录不能为空。");
        }
        return false;
    }

    const QDir outputDir(options.outputDirectory);
    if (outputDir.exists()) {
        const QStringList entries = outputDir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
        if (!entries.isEmpty()) {
            if (errorString) {
                *errorString = QStringLiteral("输出目录已存在且不是空目录，请选择新的目录。");
            }
            return false;
        }
    }

    return true;
}

bool PluginProjectValidator::isValidClassName(const QString &className)
{
    static const QRegularExpression pattern(QStringLiteral("^[A-Za-z_][A-Za-z0-9_]*$"));
    return pattern.match(className).hasMatch();
}
```

- [ ] **Step 3: Build to catch syntax errors**

Run:

```powershell
cmake --build build --config Debug --target PluginCreator
```

Expected: build fails only if later referenced UI/generator files do not exist yet. After Task 1 file list includes missing files, so a full pass is expected after Task 7.

- [ ] **Step 4: Commit**

Run:

```powershell
git add tools/PluginCreator/PluginProjectValidator.h tools/PluginCreator/PluginProjectValidator.cpp
git commit -m "添加插件创建参数校验"
```

---

## Task 4: Add Template Renderer

**Files:**
- Create: `tools/PluginCreator/TemplateRenderer.h`
- Create: `tools/PluginCreator/TemplateRenderer.cpp`

- [ ] **Step 1: Create renderer header with required comments**

Create `tools/PluginCreator/TemplateRenderer.h`:

```cpp
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
```

- [ ] **Step 2: Create renderer implementation**

Create `tools/PluginCreator/TemplateRenderer.cpp`:

```cpp
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
```

- [ ] **Step 3: Commit**

Run:

```powershell
git add tools/PluginCreator/TemplateRenderer.h tools/PluginCreator/TemplateRenderer.cpp
git commit -m "添加插件模板渲染器"
```

---

## Task 5: Add Plugin Project Generator

**Files:**
- Create: `tools/PluginCreator/PluginProjectGenerator.h`
- Create: `tools/PluginCreator/PluginProjectGenerator.cpp`

- [ ] **Step 1: Create generator header with required comments**

Create `tools/PluginCreator/PluginProjectGenerator.h`:

```cpp
#ifndef PLUGINPROJECTGENERATOR_H
#define PLUGINPROJECTGENERATOR_H

#include "PluginProjectOptions.h"

class PluginProjectGenerator
{
public:
    /*
     * 生成插件工程。
     * options 为插件创建参数，返回值包含生成状态、错误信息和文件列表。
     */
    PluginGenerationResult generate(const PluginProjectOptions &options) const;

private:
    /*
     * 生成视图插件工程文件。
     * options 为插件创建参数，result 用于记录生成文件和错误信息。
     */
    bool generateViewPlugin(const PluginProjectOptions &options, PluginGenerationResult *result) const;

    /*
     * 生成服务插件工程文件。
     * options 为插件创建参数，result 用于记录生成文件和错误信息。
     */
    bool generateServicePlugin(const PluginProjectOptions &options, PluginGenerationResult *result) const;

    /*
     * 写入文本文件。
     * filePath 为目标文件路径，content 为文件内容，result 用于记录生成结果。
     */
    bool writeTextFile(const QString &filePath, const QString &content, PluginGenerationResult *result) const;

    /*
     * 创建模板变量表。
     * options 为插件创建参数，返回值用于模板渲染。
     */
    QMap<QString, QString> variablesForOptions(const PluginProjectOptions &options) const;
};

#endif // PLUGINPROJECTGENERATOR_H
```

- [ ] **Step 2: Implement helper functions and file writing**

In `tools/PluginCreator/PluginProjectGenerator.cpp`, start with:

```cpp
#include "PluginProjectGenerator.h"
#include "PluginProjectValidator.h"
#include "TemplateRenderer.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>

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

    if (ok) {
        result.success = true;
        result.message = QStringLiteral("插件工程生成成功。");
    }

    return result;
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

QMap<QString, QString> PluginProjectGenerator::variablesForOptions(const PluginProjectOptions &options) const
{
    const QString className = options.className.trimmed();
    const QString headerGuard = className.toUpper() + QStringLiteral("_H");

    QMap<QString, QString> variables;
    variables.insert(QStringLiteral("PluginClass"), className);
    variables.insert(QStringLiteral("PluginId"), options.pluginId.trimmed());
    variables.insert(QStringLiteral("DisplayName"), options.displayName.trimmed());
    variables.insert(QStringLiteral("Version"), options.version.trimmed());
    variables.insert(QStringLiteral("Author"), options.author.trimmed());
    variables.insert(QStringLiteral("Description"), options.description.trimmed());
    variables.insert(QStringLiteral("HeaderGuard"), headerGuard);
    variables.insert(QStringLiteral("WidgetClass"), className + QStringLiteral("Widget"));
    variables.insert(QStringLiteral("WidgetHeader"), className + QStringLiteral("Widget.h"));
    variables.insert(QStringLiteral("WidgetUiHeader"), QStringLiteral("ui_") + className + QStringLiteral("Widget.h"));
    variables.insert(QStringLiteral("MetadataType"), creatorPluginTypeToMetadataType(options.type));
    variables.insert(QStringLiteral("WindowArea"), creatorWindowAreaToConfigText(options.area));
    return variables;
}
```

- [ ] **Step 3: Implement metadata generation**

Inside both `generateViewPlugin()` and `generateServicePlugin()`, create metadata with `QJsonObject`:

```cpp
QJsonObject metadata;
metadata.insert(QStringLiteral("id"), options.pluginId.trimmed());
metadata.insert(QStringLiteral("name"), options.className.trimmed());
metadata.insert(QStringLiteral("displayName"), options.displayName.trimmed());
metadata.insert(QStringLiteral("version"), options.version.trimmed());
metadata.insert(QStringLiteral("type"), creatorPluginTypeToMetadataType(options.type));
metadata.insert(QStringLiteral("enabled"), true);
metadata.insert(QStringLiteral("author"), options.author.trimmed());
metadata.insert(QStringLiteral("description"), options.description.trimmed());
```

Write it using:

```cpp
const QString metadataText = QString::fromUtf8(QJsonDocument(metadata).toJson(QJsonDocument::Indented));
```

- [ ] **Step 4: Implement complete view plugin templates**

Use embedded QString templates for:

- `${PluginClass}.h`
- `${PluginClass}.cpp`
- `${WidgetClass}.h`
- `${WidgetClass}.cpp`
- `${WidgetClass}.ui`
- `CMakeLists.txt`
- `metadata.json`

The generated plugin header must include comments like:

```cpp
/*
 * 初始化视图插件。
 * 创建插件主界面并注册插件能力，成功时返回 true。
 */
bool initialize() override;
```

The generated widget `.ui` must contain a `QWidget` root, a `QVBoxLayout`, and a centered `QLabel` with `${DisplayName}`.

- [ ] **Step 5: Implement complete service plugin templates**

Use embedded QString templates for:

- `${PluginClass}.h`
- `${PluginClass}.cpp`
- `CMakeLists.txt`
- `metadata.json`

The generated service header must include comments like:

```cpp
/*
 * 启动服务插件。
 * 在此函数中启动后台定时器、线程或业务服务。
 */
void start() override;
```

- [ ] **Step 6: Commit**

Run:

```powershell
git add tools/PluginCreator/PluginProjectGenerator.h tools/PluginCreator/PluginProjectGenerator.cpp
git commit -m "添加插件工程生成器"
```

---

## Task 6: Add Graphical Wizard UI

**Files:**
- Create: `tools/PluginCreator/PluginCreatorWindow.h`
- Create: `tools/PluginCreator/PluginCreatorWindow.cpp`
- Create: `tools/PluginCreator/PluginCreatorWindow.ui`

- [ ] **Step 1: Create window header with required comments**

Create `tools/PluginCreator/PluginCreatorWindow.h`:

```cpp
#ifndef PLUGINCREATORWINDOW_H
#define PLUGINCREATORWINDOW_H

#include "PluginProjectOptions.h"

#include <QMainWindow>

namespace Ui {
class PluginCreatorWindow;
}

class PluginCreatorWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*
     * 创建插件创建向导窗口。
     * parent 为 Qt 对象父节点，可为空。
     */
    explicit PluginCreatorWindow(QWidget *parent = nullptr);

    /*
     * 析构插件创建向导窗口。
     * 析构时释放 Qt Designer 生成的界面对象。
     */
    ~PluginCreatorWindow() override;

private slots:
    /*
     * 切换到上一页。
     * 当前已经是第一页时保持不变。
     */
    void previousPage();

    /*
     * 切换到下一页。
     * 当前页信息校验通过后进入下一页。
     */
    void nextPage();

    /*
     * 执行插件生成。
     * 从界面收集参数并调用插件工程生成器写入文件。
     */
    void generatePlugin();

    /*
     * 选择插件输出目录。
     * 打开目录选择对话框并更新输出目录输入框。
     */
    void chooseOutputDirectory();

    /*
     * 根据插件类型刷新向导页面。
     * 视图插件显示视图选项页，服务插件跳过该页面。
     */
    void updatePluginTypeUi();

private:
    /*
     * 初始化界面默认状态。
     * 设置中文标题、默认字段值和按钮连接。
     */
    void setupDefaults();

    /*
     * 从界面收集插件创建参数。
     * 返回值用于校验和生成插件工程。
     */
    PluginProjectOptions collectOptions() const;

    /*
     * 刷新向导按钮状态。
     * 根据当前页决定上一步、下一步和生成按钮是否可用。
     */
    void updateButtons();

    Ui::PluginCreatorWindow *ui = nullptr;
};

#endif // PLUGINCREATORWINDOW_H
```

- [ ] **Step 2: Create window implementation**

Create `tools/PluginCreator/PluginCreatorWindow.cpp` with:

```cpp
#include "PluginCreatorWindow.h"
#include "PluginProjectGenerator.h"
#include "PluginProjectValidator.h"
#include "ui_PluginCreatorWindow.h"

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

PluginCreatorWindow::PluginCreatorWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PluginCreatorWindow)
{
    ui->setupUi(this);
    setupDefaults();
}

PluginCreatorWindow::~PluginCreatorWindow()
{
    delete ui;
}
```

Then implement the declared functions using `QStackedWidget`, `QLineEdit`, `QComboBox`, `QCheckBox`, `QTextEdit`, and `QPushButton` object names from the `.ui` file.

- [ ] **Step 3: Create UI file**

Create `tools/PluginCreator/PluginCreatorWindow.ui` with these object names:

- `pageStack`
- `viewPluginRadio`
- `servicePluginRadio`
- `classNameEdit`
- `pluginIdEdit`
- `displayNameEdit`
- `versionEdit`
- `authorEdit`
- `descriptionEdit`
- `outputDirectoryEdit`
- `chooseDirectoryButton`
- `areaComboBox`
- `registerCapabilityCheck`
- `messageBusCheck`
- `logServiceCheck`
- `pluginSettingsCheck`
- `summaryTextEdit`
- `resultTextEdit`
- `previousButton`
- `nextButton`
- `generateButton`
- `closeButton`

The UI text must be Chinese. Window title must be `插件创建向导`.

- [ ] **Step 4: Build the tool**

Run:

```powershell
cmake --build build --config Debug --target PluginCreator
```

Expected: `build/bin/Debug/PluginCreator.exe` is created.

- [ ] **Step 5: Commit**

Run:

```powershell
git add tools/PluginCreator/PluginCreatorWindow.h tools/PluginCreator/PluginCreatorWindow.cpp tools/PluginCreator/PluginCreatorWindow.ui
git commit -m "添加图形化插件创建向导"
```

---

## Task 7: Manual Generation Verification

**Files:**
- No committed test files.
- Generated sample plugins must be created under a temporary ignored directory or deleted before commit.

- [ ] **Step 1: Start the creator**

Run:

```powershell
.\build\bin\Debug\PluginCreator.exe
```

Expected: The graphical wizard opens with Chinese UI.

- [ ] **Step 2: Generate a view plugin**

Use these values:

```text
插件类型：视图插件
插件类名：SampleViewPlugin
插件 ID：com.pluginframe.sample-view
显示名称：示例视图
版本号：1.0.0
输出目录：F:\QtCode_5.15.2\MrkPluginFrame\plugins\SampleViewPlugin
默认窗口区域：中心区
```

Expected generated files:

```text
plugins/SampleViewPlugin/CMakeLists.txt
plugins/SampleViewPlugin/metadata.json
plugins/SampleViewPlugin/SampleViewPlugin.h
plugins/SampleViewPlugin/SampleViewPlugin.cpp
plugins/SampleViewPlugin/SampleViewPluginWidget.h
plugins/SampleViewPlugin/SampleViewPluginWidget.cpp
plugins/SampleViewPlugin/SampleViewPluginWidget.ui
```

- [ ] **Step 3: Verify generated view plugin compiles**

Temporarily add this line to root `CMakeLists.txt`:

```cmake
add_subdirectory(plugins/SampleViewPlugin)
```

Run:

```powershell
cmake -S . -B build
cmake --build build --config Debug
```

Expected: The generated view plugin compiles.

- [ ] **Step 4: Clean manual verification changes**

Remove this temporary line from root `CMakeLists.txt`:

```cmake
add_subdirectory(plugins/SampleViewPlugin)
```

Delete the generated `plugins/SampleViewPlugin` directory after verification.

- [ ] **Step 5: Generate and verify a service plugin**

Repeat the same flow with:

```text
插件类型：服务插件
插件类名：SampleServicePlugin
插件 ID：com.pluginframe.sample-service
显示名称：示例服务
版本号：1.0.0
输出目录：F:\QtCode_5.15.2\MrkPluginFrame\plugins\SampleServicePlugin
```

Temporarily add:

```cmake
add_subdirectory(plugins/SampleServicePlugin)
```

Run configure and build, then remove the temporary line and generated directory.

- [ ] **Step 6: Commit verification fixes if needed**

If verification reveals generator bugs, fix them and commit:

```powershell
git add tools/PluginCreator
git commit -m "修复插件创建器生成结果"
```

---

## Task 8: Update Documentation

**Files:**
- Modify: `README.md`
- Modify: `docs/框架使用说明.md`

- [ ] **Step 1: Add README section**

Add a section named `插件创建向导` to `README.md`:

```markdown
## 插件创建向导

框架提供 `PluginCreator.exe` 图形化插件创建向导，可用于生成标准视图插件和服务插件。

构建：

```powershell
cmake --build build --config Debug --target PluginCreator
```

运行：

```powershell
.\build\bin\Debug\PluginCreator.exe
```

视图插件默认生成独立 QWidget 和 `.ui` 文件。服务插件默认生成无界面的后台服务模板。
```

- [ ] **Step 2: Add detailed docs**

In `docs/框架使用说明.md`, add:

```markdown
## 插件创建向导

`PluginCreator.exe` 是框架自带的图形化插件创建工具。它用于生成符合当前框架规范的插件工程。

当前支持：

- 视图插件：生成插件类、独立 QWidget、`.ui` 文件、元数据和 CMake 文件。
- 服务插件：生成服务插件类、元数据和 CMake 文件。

生成完成后，需要按照工具提示在根目录 `CMakeLists.txt` 中添加：

```cmake
add_subdirectory(plugins/插件目录名)
```

生成的头文件已经包含框架要求的函数块注释，后续业务开发时应继续保持该注释风格。
```

- [ ] **Step 3: Commit**

Run:

```powershell
git add README.md docs/框架使用说明.md
git commit -m "补充插件创建向导使用说明"
```

---

## Task 9: Final Verification

**Files:**
- No new files unless fixing verification bugs.

- [ ] **Step 1: Check working tree**

Run:

```powershell
git status --short
```

Expected: only known user-local changes remain. Do not commit `res/` or unrelated user edits unless explicitly requested.

- [ ] **Step 2: Full build**

Run:

```powershell
cmake -S . -B build
cmake --build build --config Debug
```

Expected: `PluginHost` and `PluginCreator` both build successfully.

- [ ] **Step 3: Run host**

Run:

```powershell
.\build\bin\Debug\PluginHost.exe
```

Expected: Existing host UI still opens and existing plugins still load.

- [ ] **Step 4: Run creator**

Run:

```powershell
.\build\bin\Debug\PluginCreator.exe
```

Expected: The creator opens, validates fields in Chinese, and generates plugins without overwriting non-empty directories.

- [ ] **Step 5: Push if requested**

Only push when the user asks. Use Chinese commit messages for all commits.
