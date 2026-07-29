# PluginFrame

基于 Qt 5 和 CMake 的微内核插件框架原型。

## 功能

- `framework` 接口层：`IPlugin`、`IViewPlugin`、`IServicePlugin`
- 插件生命周期管理：`Loaded`、`Initialized`、`Started`、`Stopped`、`Disabled`、`Failed`
- 内部通信：`MessageBus` + `IPluginContext`
- 插件元数据：`id`、`name`、`displayName`、`version`、`type`、`enabled`
- 主程序自动加载 `plugins` 目录下的动态库
- 示例视图插件：`HelloPlugin`

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
