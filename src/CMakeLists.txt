add_library(plugin_host_lib
    AppConfig.cpp
    AppConfig.h
    CyberSidebar.cpp
    CyberSidebar.h
    LogViewerWidget.cpp
    LogViewerWidget.h
    StatusBarWidget.cpp
    StatusBarWidget.h
    ThemeManager.cpp
    ThemeManager.h
    TitleBar.cpp
    TitleBar.h
    WindowManager.cpp
    WindowManager.h
    PluginMetadata.cpp
    PluginMetadata.h
    PluginContext.cpp
    PluginContext.h
    PluginInspectorWidget.cpp
    PluginInspectorWidget.h
    PluginManager.cpp
    PluginManager.h
    MainWindow.cpp
    MainWindow.h
    resources.qrc
)

target_include_directories(plugin_host_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(plugin_host_lib
    PUBLIC
        Qt5::Core
        Qt5::Widgets
        Qt5::Xml
        framework
)

add_executable(PluginHost
    main.cpp
)

target_link_libraries(PluginHost
    PRIVATE
        plugin_host_lib
)

set_target_properties(PluginHost PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

add_custom_command(TARGET PluginHost POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/themes
            $<TARGET_FILE_DIR:PluginHost>/themes
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/config
            $<TARGET_FILE_DIR:PluginHost>/config
)
