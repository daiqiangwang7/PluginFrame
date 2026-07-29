# PluginFrame

基于 Qt 5 和 CMake 的微内核插件框架原型。

## 功能

- `framework` 接口层：`IPlugin`、`IViewPlugin`、`IServicePlugin`
- 插件生命周期管理：`Loaded`、`Initialized`、`Started`、`Stopped`、`Disabled`、`Failed`
- 内部通信：`MessageBus` + `IPluginContext`
- 插件元数据：`id`、`name`、`displayName`、`version`、`type`、`enabled`
- 主程序自动加载 `plugins` 目录下的动态库
- 内置插件诊断页：展示插件名称、类型、版本、状态、路径和错误信息
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

## 插件元数据

每个插件通过 `Q_PLUGIN_METADATA` 引用同目录下的 `metadata.json`。

```json
{
  "id": "com.pluginframe.hello",
  "name": "HelloPlugin",
  "displayName": "Hello Plugin",
  "version": "1.0.0",
  "type": "view",
  "enabled": true
}
```

字段说明：

- `id`：插件唯一标识。
- `name`：插件内部名称。
- `displayName`：插件显示名称。
- `version`：插件版本。
- `type`：插件类型，目前支持 `view` 和 `service`。
- `enabled`：是否启用插件，`false` 时主程序会跳过加载。

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

主程序内置 `Plugins` 标签页，用于查看当前插件加载情况。

表格包含：

- `Name`：插件名称。
- `Type`：插件类型。
- `Version`：插件版本。
- `State`：插件生命周期状态。
- `Path`：插件动态库路径。
- `Error`：插件加载或元数据解析失败原因。
