add_library(HelloPlugin MODULE
    HelloPlugin.cpp
    HelloPlugin.h
    metadata.json
)

target_link_libraries(HelloPlugin
    PRIVATE
        Qt5::Core
        Qt5::Widgets
        framework
)

set_target_properties(HelloPlugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/plugins
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/plugins
)

add_custom_command(TARGET HelloPlugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:PluginHost>/plugins
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:HelloPlugin>
            $<TARGET_FILE_DIR:PluginHost>/plugins/$<TARGET_FILE_NAME:HelloPlugin>
)
