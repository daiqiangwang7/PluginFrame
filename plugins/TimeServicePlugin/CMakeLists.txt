add_library(TimeServicePlugin MODULE
    TimeServicePlugin.cpp
    TimeServicePlugin.h
    metadata.json
)

target_link_libraries(TimeServicePlugin
    PRIVATE
        Qt5::Core
        framework
)

set_target_properties(TimeServicePlugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/plugins
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/plugins
)

add_custom_command(TARGET TimeServicePlugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:PluginHost>/plugins
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:TimeServicePlugin>
            $<TARGET_FILE_DIR:PluginHost>/plugins/$<TARGET_FILE_NAME:TimeServicePlugin>
)
