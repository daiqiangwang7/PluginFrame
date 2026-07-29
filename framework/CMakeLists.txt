add_library(framework_moc STATIC
    IPlugin.h
    IPluginContext.h
    IViewPlugin.h
    IServicePlugin.h
    MessageBus.cpp
    MessageBus.h
)

target_include_directories(framework_moc
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(framework_moc
    PUBLIC
        Qt5::Core
        Qt5::Widgets
)

add_library(framework INTERFACE)

target_include_directories(framework
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(framework
    INTERFACE
        framework_moc
)
