# PluginCreator 图形插件创建向导设计

## 目标

为 PluginFrame 增加一个 Qt 图形化插件创建向导，用于自动生成视图插件和服务插件的标准工程代码。工具本身作为框架 SDK 的一部分，帮助后续插件开发保持统一目录结构、统一注释规范、统一 CMake 写法和统一元数据格式。

## 设计原则

1. 插件创建工具只负责生成代码，不参与插件运行。
2. 生成结果必须能直接加入当前框架构建。
3. 生成的头文件函数声明必须使用框架约定的块注释风格，并写清楚函数功能。
4. 视图插件默认生成独立 QWidget 和 `.ui` 文件。
5. 服务插件不生成界面文件，只生成后台服务模板。
6. 工具先支持当前项目内生成，后续保留任意目录输出能力。

## 程序形态

新增一个独立可执行程序：

```text
PluginCreator.exe
```

该程序使用 Qt Widgets 开发，和主程序共用 Qt 5、CMake、主题资源和部分基础工具类。它不加载业务插件，不依赖 `PluginManager`，只依赖模板渲染和文件生成逻辑。

不建议拆成 `CreateViewPlugin.exe` 和 `CreateServicePlugin.exe` 两个程序。统一工具更方便后续扩展设置页插件、工具栏插件、数据源插件等新模板。

## 页面流程

### 第一步：选择插件类型

用户选择需要创建的插件类型：

- 视图插件
- 服务插件

视图插件用于生成带 UI 的插件。服务插件用于生成后台运行的无 UI 插件。

### 第二步：填写插件信息

表单字段：

- 插件类名：例如 `DeviceMonitorPlugin`
- 插件 ID：例如 `com.pluginframe.device-monitor`
- 显示名称：例如 `设备监控`
- 版本号：默认 `1.0.0`
- 作者：可为空
- 描述：可为空
- 输出目录：默认当前项目 `plugins/<插件类名>/`

基础校验规则：

- 插件类名必须是合法 C++ 类名。
- 插件 ID 不能为空，建议使用反域名格式。
- 显示名称不能为空。
- 输出目录不存在时可以创建。
- 输出目录已存在且非空时禁止覆盖，除非后续增加明确的覆盖确认流程。

### 第三步：视图插件选项

仅视图插件显示该页面。

配置项：

- 默认窗口区域：中心区、左侧区、右侧区、底部区、浮动窗口。
- 是否生成 `.ui` 文件：默认启用。
- 是否生成独立视图类：默认启用。

第一版固定采用独立视图类结构，不提供关闭选项，避免模板分支过多。

### 第四步：公共能力选项

视图插件和服务插件都显示该页面。

可选项：

- 注册能力：默认启用。
- 使用消息总线：默认启用。
- 使用日志服务：默认启用。
- 使用插件私有配置：默认启用。

这些选项影响生成的示例代码，但不会影响框架接口。

### 第五步：确认并生成

确认页展示：

- 插件类型。
- 插件类名。
- 插件 ID。
- 输出目录。
- 将生成的文件列表。
- 需要添加到根 `CMakeLists.txt` 的 `add_subdirectory(...)` 提示。

点击生成后，工具创建文件并显示结果。

## 生成文件结构

### 视图插件

视图插件默认生成：

```text
plugins/MyViewPlugin/
  CMakeLists.txt
  metadata.json
  MyViewPlugin.h
  MyViewPlugin.cpp
  MyViewWidget.h
  MyViewWidget.cpp
  MyViewWidget.ui
```

职责划分：

- `MyViewPlugin.h/.cpp`：负责插件生命周期、上下文保存、能力注册、返回主界面。
- `MyViewWidget.h/.cpp/.ui`：负责具体界面实现。
- `metadata.json`：保存插件 ID、名称、显示名称、版本、类型、启用状态和依赖。
- `CMakeLists.txt`：定义插件动态库目标，并启用 Qt 自动处理 `.ui` 文件。

### 服务插件

服务插件默认生成：

```text
plugins/MyServicePlugin/
  CMakeLists.txt
  metadata.json
  MyServicePlugin.h
  MyServicePlugin.cpp
```

职责划分：

- `MyServicePlugin.h/.cpp`：负责插件生命周期、后台对象初始化、服务启动和停止。
- `metadata.json`：保存服务插件元数据。
- `CMakeLists.txt`：定义服务插件动态库目标。

## 模板渲染设计

第一版使用内置文本模板，不引入第三方模板库。

推荐新增模板渲染类：

```text
tools/PluginCreator/TemplateRenderer.h
tools/PluginCreator/TemplateRenderer.cpp
```

职责：

- 接收模板文本和变量表。
- 将 `${PluginClass}`、`${PluginId}`、`${DisplayName}` 等占位符替换为实际值。
- 返回渲染后的文本。

第一版只允许简单占位符替换，不支持条件语法和循环语法。这样实现简单，也便于后续替换为文件模板。

## 文件生成设计

推荐新增文件生成类：

```text
tools/PluginCreator/PluginProjectGenerator.h
tools/PluginCreator/PluginProjectGenerator.cpp
```

职责：

- 校验插件信息。
- 创建输出目录。
- 调用模板渲染器生成文件内容。
- 写入 `.h`、`.cpp`、`.ui`、`metadata.json`、`CMakeLists.txt`。
- 返回成功或失败结果。

生成逻辑不直接放在界面类中，避免 UI 和文件生成耦合。

## UI 类设计

推荐新增主窗口：

```text
tools/PluginCreator/PluginCreatorWindow.h
tools/PluginCreator/PluginCreatorWindow.cpp
tools/PluginCreator/PluginCreatorWindow.ui
```

第一版可以使用 `QStackedWidget` 实现向导页面：

- 类型选择页。
- 基础信息页。
- 视图选项页。
- 公共能力页。
- 确认生成页。
- 生成结果页。

底部按钮：

- 上一步
- 下一步
- 生成
- 关闭

## 错误处理

工具必须处理以下错误：

- 插件类名为空或非法。
- 插件 ID 为空。
- 显示名称为空。
- 输出目录创建失败。
- 输出目录已存在且非空。
- 文件写入失败。
- 模板渲染缺少变量。

错误以中文消息显示，并且不能生成半成品。如果生成过程中部分文件已经写入但后续失败，第一版可以提示用户删除目录后重试；后续再增加自动回滚。

## 和现有框架的关系

插件创建器需要遵守现有框架结构：

- 插件接口来自 `framework/`。
- 视图插件继承 `IViewPlugin`。
- 服务插件继承 `IServicePlugin`。
- 插件通过 `IPluginContext` 使用消息总线、能力注册、插件配置和日志服务。
- 插件元数据格式沿用当前 `metadata.json`。
- 生成的 CMake 写法必须和现有 `HelloPlugin`、`TimeServicePlugin` 保持一致。

## 不包含内容

第一版不做以下功能：

- 自动修改根目录 `CMakeLists.txt`。
- 自动编译插件。
- 自动安装插件。
- 插件图标选择。
- 插件依赖选择器。
- 模板市场。
- 插件热更新。

这些能力后续可以继续扩展。

## 验收标准

1. 可以启动 `PluginCreator.exe`。
2. 可以通过图形界面创建视图插件。
3. 视图插件生成 `.ui` 文件，并能被 CMake 编译。
4. 可以通过图形界面创建服务插件。
5. 服务插件能被 CMake 编译。
6. 生成的头文件函数声明包含明确功能说明的块注释。
7. 生成的 `metadata.json` 格式符合当前框架。
8. 生成失败时给出中文错误提示。
9. 工具不会覆盖已有非空插件目录。
10. 生成完成后提示用户需要添加的 `add_subdirectory(...)`。

## 后续扩展方向

后续可以继续增加：

- 自动修改根 `CMakeLists.txt`。
- 插件依赖选择。
- 插件图标和资源生成。
- 插件翻译文件生成。
- 设置页插件模板。
- 命令插件模板。
- 数据源插件模板。
- 插件打包向导。
