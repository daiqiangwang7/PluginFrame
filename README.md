# PluginFrame

基于 Qt 5 和 CMake 的微内核插件框架原型。

## 功能

- `framework` 接口层：`IPlugin`、`IViewPlugin`、`IServicePlugin`
- 插件生命周期管理：`Loaded`、`Initialized`、`Started`、`Stopped`、`Failed`
- 内部通信：`MessageBus` + `IPluginContext`
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
