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

set_source_files_properties(PluginProjectGenerator.cpp PROPERTIES SKIP_AUTOMOC ON)

target_link_libraries(PluginCreator
    PRIVATE
        Qt5::Core
        Qt5::Widgets
)

set_target_properties(PluginCreator PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
