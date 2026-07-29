add_library(plugin_host_lib
    PluginMetadata.cpp
    PluginMetadata.h
    PluginContext.cpp
    PluginContext.h
    PluginManager.cpp
    PluginManager.h
    MainWindow.cpp
    MainWindow.h
)

target_include_directories(plugin_host_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(plugin_host_lib
    PUBLIC
        Qt5::Core
        Qt5::Widgets
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
