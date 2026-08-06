# PluginFrame

基于 Qt 5 和 CMake 的微内核插件框架原型。

## 功能

- `framework` 接口层：`IPlugin`、`IViewPlugin`、`IServicePlugin`
- 插件生命周期管理：`Loaded`、`Initialized`、`Started`、`Stopped`、`Disabled`、`Failed`
- 内部通信：`MessageBus` + `IPluginContext`
- 能力注册中心：插件可注册视图、服务、命令、设置页等能力
- 插件私有配置：插件可通过上下文读写自己的 ini 配置文件
- 统一日志服务：宿主和插件可写入日志，日志页实时显示并落盘到 `logs/app.log`
- 插件元数据：`id`、`name`、`displayName`、`version`、`type`、`enabled`
- 插件依赖：通过 `dependencies` 声明依赖关系，宿主按依赖顺序启动插件
- 主程序自动加载 `plugins` 目录下的动态库
- 内置插件诊断页：展示插件名称、类型、版本、状态、路径和错误信息
- 多区域窗口管理：中心区、左侧区、右侧区、底部区和浮动窗口
- 右侧贴边抽屉：通过屏幕右侧把手展开总览、插件、服务、日志、设置入口
- 底部状态栏：显示主题、插件数量、服务数量和当前时间
- 自绘赛博朋克风格主窗口和标题栏
- 内置深色、浅色两套赛博朋克主题
- 默认中文界面
- 示例视图插件：`HelloPlugin`
- 示例服务插件：`TimeServicePlugin`

## 构建

```powershell
cmake -S . -B build
cmake --build build --config Debug
```

构建完成后运行：

```powershell
.\build\bin\Debug\PluginHost.exe
```

插件会复制到：

```text
build/bin/Debug/plugins/
```

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

视图插件默认生成独立 QWidget 和 `.ui` 文件。服务插件默认生成无界面的后台服务模板。生成完成后，工具默认会自动把插件目录写入根目录 `CMakeLists.txt`，让新插件直接加入工程构建。

## 插件元数据

每个插件通过 `Q_PLUGIN_METADATA` 引用同目录下的 `metadata.json`。

```json
{
  "id": "com.pluginframe.hello",
  "name": "HelloPlugin",
  "displayName": "Hello Plugin",
  "version": "1.0.0",
  "type": "view",
  "dependencies": [
    "com.pluginframe.time-service"
  ],
  "enabled": true
}
```

字段说明：

- `id`：插件唯一标识。
- `name`：插件内部名称。
- `displayName`：插件显示名称。
- `version`：插件版本。
- `type`：插件类型，目前支持 `view` 和 `service`。
- `dependencies`：依赖插件 id 数组，宿主会先启动依赖插件。
- `enabled`：是否启用插件，`false` 时主程序会跳过加载。

## 能力注册

插件可通过 `IPluginContext::capabilityRegistry()` 注册能力。能力用于描述插件向宿主提供的扩展点，例如：

- `view`：视图窗口。
- `service`：后台服务能力。
- `command`：命令或操作入口。
- `settings`：插件设置页。

示例：

```cpp
m_context->capabilityRegistry()->registerCapability({
    QStringLiteral("com.pluginframe.hello"),
    QStringLiteral("hello.view"),
    QStringLiteral("view"),
    QStringLiteral("你好插件视图"),
    QStringLiteral("HelloPlugin")
});
```

## 插件配置

插件可通过 `IPluginContext::pluginSettings()` 读写私有配置。宿主默认将配置保存到：

```text
config/plugins/<pluginId>.ini
```

示例：

```cpp
const QString pluginId = QStringLiteral("com.pluginframe.hello");
m_context->pluginSettings()->setValue(pluginId, QStringLiteral("ui/greeting"),
                                      QStringLiteral("你好，来自视图插件！"));
const QString greeting = m_context->pluginSettings()
        ->value(pluginId, QStringLiteral("ui/greeting")).toString();
```

当前示例插件使用了配置服务：

- `HelloPlugin`：保存并读取 `ui/greeting`。
- `TimeServicePlugin`：保存并读取 `timer/intervalMs`。

## 日志服务

插件可通过 `IPluginContext::logService()` 写入统一日志。日志支持 `Debug`、`Info`、`Warning`、`Error` 等等级，默认保存到：

```text
logs/app.log
```

右侧抽屉点击 `日志` 可打开内置运行日志窗口。

## 示例插件

### HelloPlugin

`HelloPlugin` 是一个视图插件，提供一个标签页界面。它会订阅：

- `app.status`：显示宿主程序状态。
- `time.tick`：显示时间服务发布的当前时间。

点击界面按钮时，它会发布：

- `hello.clicked`

### TimeServicePlugin

`TimeServicePlugin` 是一个无 UI 的服务插件。插件启动后会启动一个 `QTimer`，每秒发布：

```text
topic: time.tick
payload:
  time: yyyy-MM-dd HH:mm:ss
```

## 插件诊断页

主程序内置 `插件诊断` 窗口，默认注册到底部区域，用于查看当前插件加载情况。

表格包含：

- `Name`：插件名称。
- `Type`：插件类型。
- `Version`：插件版本。
- `State`：插件生命周期状态。
- `Path`：插件动态库路径。
- `Error`：插件加载或元数据解析失败原因。

## 界面主题

主程序使用自绘标题栏，不使用 Qt 默认系统标题栏。标题栏提供：

- 皮肤切换
- 皮肤按钮使用深色、浅色主题各自适配的 PNG 图标
- 图标化最小化
- 图标化最大化/还原
- 图标化关闭
- 拖动窗口

内置主题文件位于：

```text
themes/cyber-dark.qss
themes/cyber-light.qss
```

默认主题为 `cyber-dark`，界面默认使用中文文案。

## 窗口管理

主程序通过 `WindowManager` 管理界面区域，不再依赖单一 `QTabWidget` 显示所有内容。

当前支持区域：

- `Central`：中心工作区，默认承载视图插件。
- `LeftDock`：左侧停靠区。
- `RightDock`：右侧停靠区。
- `BottomDock`：底部停靠区，默认承载插件诊断窗口。
- `Floating`：独立浮动窗口。

主界面还内置：

- `CyberSidebar`：右侧贴边抽屉，点击屏幕右侧把手展开或收起。
- `StatusBarWidget`：底部运行状态栏。

## XML 配置

框架启动时会读取：

```text
config/app.xml
```

当前支持配置：

- 默认主题
- 插件启用状态
- 视图插件注册区域

示例：

```xml
<application>
  <theme default="cyber-dark"/>

  <plugins>
    <plugin id="com.pluginframe.hello" enabled="true" area="central"/>
    <plugin id="com.pluginframe.time-service" enabled="true"/>
  </plugins>
</application>
```

`area` 支持：

- `central`
- `leftDock`
- `rightDock`
- `bottomDock`
- `floating`
