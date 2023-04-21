cmake_minimum_required(VERSION 3.12)

#
# MOUSETRAP: GUI Library
#
# Dependencies:
#   GTK4
#   OpenGL
#   GLEW
#   GLM (OpenGL Math)
#
# Variables:
#   MOUSETRAP_BUILD_TESTS: boolean
#       Whether to build tests
#
#   MOUSETRAP_BUILD_DOCUMENTATION: boolean
#       Whether to build the documentation.
#       Additional Dependencies:
#           Doxygen
#
#   MOUSETRAP_BUILD_JULIA_BINDING: boolean
#       Whether to generate the Julia Binding
#       Additional Dependencies:
#           Julia
#           JLxcc
#

### PROJECT ###

project(mousetrap
    VERSION 0.1.0
    DESCRIPTION "TODO" # TODO
    LANGUAGES CXX
)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(PROJECT_PREFIX "")#"mousetrap_") # TODO

### DOCUMENTATION ###

option(MOUSETRAP_BUILD_DOCUMENTATION OFF)
if (${MOUSETRAP_BUILD_DOCUMENTATION})

    find_package(Doxygen REQUIRED)
    set(PROJECT_DOCUMENTATION_DIR "${CMAKE_SOURCE_DIR}/docs")
    if (NOT EXISTS "${PROJECT_DOCUMENTATION_DIR}")
        message(WARNING "Directory mousetrap/docs is missing, unable to build documentation.")
    else()
        configure_file(
            "${PROJECT_DOCUMENTATION_DIR}/Doxyfile.in"
            "${PROJECT_DOCUMENTATION_DIR}/Doxyfile"
            @ONLY
        )
        add_custom_target("${PROJECT_PREFIX}docs"
            COMMAND doxygen
            WORKING_DIRECTORY "${PROJECT_DOCUMENTATION_DIR}"
        )
    endif()
endif()

### DEPENDENCIES ###

find_library(OpenGL REQUIRED NAMES GL)
find_library(GLEW REQUIRED NAMES GLEW)
find_package(SFML COMPONENTS audio REQUIRED)

# GTK4
find_package(PkgConfig)
pkg_check_modules(GTK gtk4)

if (NOT GTK_FOUND)
    message(FATAL_ERROR "Missing Dependency: unable to locate GTK4")
endif()

# OpenGL Math
include(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX(glm/glm.hpp GLM_FOUND)
if(NOT GLM_FOUND)
    message(FATAL_ERROR "Missing Dependency: unable to locate GLM")
endif()

### MOUSETRAP ##

set(MOUSETRAP_HEADER_FILES
    include/action.hpp
    include/adjustment.hpp
    include/alignment.hpp
    include/angle.hpp
    include/application.hpp
    include/aspect_frame.hpp
    include/blend_mode.hpp
    include/box.hpp
    include/button.hpp
    include/center_box.hpp
    include/check_button.hpp
    include/click_event_controller.hpp
    include/clipboard.hpp
    include/color.hpp
    include/column_view.hpp
    include/cursor_type.hpp
    include/drag_event_controller.hpp
    include/drop_down.hpp
    include/entry.hpp
    include/event_controller.hpp
    include/expander.hpp
    include/file_chooser.hpp
    include/file_descriptor.hpp
    include/file_monitor.hpp
    include/file_system.hpp
    include/fixed.hpp
    include/focus_event_controller.hpp
    include/frame.hpp
    include/frame_clock.hpp
    include/geometry.hpp
    include/gl_common.hpp
    include/gl_transform.hpp
    include/grid.hpp
    include/grid_view.hpp
    include/header_bar.hpp
    include/icon.hpp
    include/image.hpp
    include/image_display.hpp
    include/justify_mode.hpp
    include/key_event_controller.hpp
    include/key_file.hpp
    include/label.hpp
    include/level_bar.hpp
    include/list_view.hpp
    include/log.hpp
    include/long_press_event_controller.hpp
    include/menu_model.hpp
    include/menubar.hpp
    include/motion_event_controller.hpp
    include/msaa_render_texture.hpp
    include/music.hpp
    include/music.hpp
    include/notebook.hpp
    include/orientable.hpp
    include/orientation.hpp
    include/overlay.hpp
    include/pan_event_controller.hpp
    include/paned.hpp
    include/pinch_zoom_event_controller.hpp
    include/popover.hpp
    include/popover_menu_button.hpp
    include/progress_bar.hpp
    include/relative_position.hpp
    include/render_area.hpp
    include/render_task.hpp
    include/render_texture.hpp
    include/revealer.hpp
    include/rotate_event_controller.hpp
    include/scale.hpp
    include/scale_mode.hpp
    include/scroll_event_controller.hpp
    include/scrollbar.hpp
    include/scrolled_window.hpp
    include/selectable.hpp
    include/selection_model.hpp
    include/separator.hpp
    include/shader.hpp
    include/shortcut_controller.hpp
    include/signal_component.hpp
    include/signal_emitter.hpp
    include/sound.hpp
    include/sound_buffer.hpp
    include/spin_button.hpp
    include/spinner.hpp
    include/stack.hpp
    include/stylus_event_controller.hpp
    include/swipe_event_controller.hpp
    include/switch.hpp
    include/text_view.hpp
    include/texture.hpp
    include/texture_object.hpp
    include/time.hpp
    include/toggle_button.hpp
    include/transition_type.hpp
    include/vector.hpp
    include/widget.hpp
    include/window.hpp
    include/wrap_mode.hpp
    include/shape.hpp
)

set(MOUSETRAP_SOURCE_FILES
    src/action.inl
    src/adjustment.cpp
    src/angle.cpp
    src/application.cpp
    src/aspect_frame.cpp
    src/blend_mode.cpp
    src/box.cpp
    src/button.cpp
    src/center_box.cpp
    src/check_button.cpp
    src/click_event_controller.cpp
    src/clipboard.cpp
    src/clipboard.inl
    src/color.cpp
    src/column_view.cpp
    src/column_view.inl
    src/drag_event_controller.cpp
    src/drop_down.cpp
    src/drop_down.inl
    src/entry.cpp
    src/event_controller.cpp
    src/expander.cpp
    src/file_chooser.cpp
    src/file_chooser.cpp
    src/file_descriptor.cpp
    src/file_monitor.cpp
    src/file_system.cpp
    src/fixed.cpp
    src/focus_event_controller.cpp
    src/frame.cpp
    src/frame_clock.cpp
    src/geometry.cpp
    src/gl_common.cpp
    src/gl_transform.cpp
    src/grid.cpp
    src/grid_view.cpp
    src/header_bar.cpp
    src/icon.cpp
    src/image.cpp
    src/image_display.cpp
    src/key_event_controller.cpp
    src/key_file.cpp
    src/label.cpp
    src/level_bar.cpp
    src/list_view.cpp
    src/log.cpp
    src/log.inl
    src/long_press_event_controller.cpp
    src/menu_bar.cpp
    src/menu_model.cpp
    src/motion_event_controller.cpp
    src/msaa_render_texture.cpp
    src/music.cpp
    src/music.cpp
    src/notebook.cpp
    src/overlay.cpp
    src/pan_event_controller.cpp
    src/paned.cpp
    src/pinch_zoom_event_controller.cpp
    src/popover.cpp
    src/popover_menu_button.cpp
    src/progress_bar.cpp
    src/render_area.cpp
    src/render_task.cpp
    src/render_texture.cpp
    src/revealer.cpp
    src/rotate_event_controller.cpp
    src/scale.cpp
    src/scale.inl
    src/scroll_event_controller.cpp
    src/scrollbar.cpp
    src/scrolled_window.cpp
    src/selection_model.cpp
    src/separator.cpp
    src/shader.cpp
    src/shape.cpp
    src/shortcut_controller.cpp
    src/signal_component.cpp
    src/signal_emitter.cpp
    src/signal_emitter.inl
    src/sound.cpp
    src/sound_buffer.cpp
    src/spin_button.cpp
    src/spin_button.inl
    src/spinner.cpp
    src/stack.cpp
    src/stylus_event_controller.cpp
    src/swipe_event_controller.cpp
    src/switch.cpp
    src/text_view.cpp
    src/texture.cpp
    src/time.cpp
    src/toggle_button.cpp
    src/widget.cpp
    src/widget.inl
    src/window.cpp
    src/action.cpp
)

add_library(mousetrap STATIC
    ${MOUSETRAP_HEADER_FILES}
    ${MOUSETRAP_SOURCE_FILES}
)

target_include_directories(mousetrap PUBLIC
    "${CMAKE_SOURCE_DIR}"
    "${GTK_INCLUDE_DIRS}"
)

target_link_libraries(mousetrap PUBLIC
    ${OpenGL}
    ${GLEW}
    ${GTK_LIBRARIES}
    sfml-audio
)

target_compile_features(mousetrap PUBLIC
    cxx_std_17
)

set_target_properties(mousetrap PROPERTIES
    LINKER_LANGUAGE CXX
    POSITION_INDEPENDENT_CODE ON
    INTERFACE_INCLUDE_DIRECTORIES "${GTK_INCLUDE_DIRS}"
    INTERFACE_LINK_LIBRARIES "${OpenGL};${GLEW};${GTK_LIBRARIES}"
)

### TEST ###

option(MOUSETRAP_BUILD_TESTS ON)
if(MOUSETRAP_BUILD_TESTS)
    if (NOT EXISTS "${CMAKE_SOURCE_DIR}/test")
        message(WARNING "Directory mousetrap/test is missing, unable to build tests.")
    else()
        set(TEST_PREFIX "test_")
        function(declare_test name)
            add_executable(
                "${PROJECT_PREFIX}${TEST_PREFIX}${name}"
                "${CMAKE_SOURCE_DIR}/test/${name}.cpp"
            )
            target_link_libraries("${PROJECT_PREFIX}${TEST_PREFIX}${name}" PRIVATE mousetrap)
            add_test(NAME "${PROJECT_PREFIX}${TEST_PREFIX}${name}" COMMAND "${PROJECT_PREFIX}${TEST_PREFIX}${name}")
        endfunction()

        declare_test(debug)
        declare_test(main)
    endif()
endif()

### INSTALL MOUSETRAP ###

set(MOUSETRAP_CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE STRING "install path for jlcxxConfig.cmake")

install(TARGETS mousetrap EXPORT mousetrap
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mousetrap
)

include(CMakePackageConfigHelpers)
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION ${MOUSETRAP_CMAKECONFIG_INSTALL_DIR}
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${${PROJECT_NAME}_VERSION}
    COMPATIBILITY AnyNewerVersion
)

export(TARGETS mousetrap
    NAMESPACE mousetrap::
    FILE mousetrapTargets.cmake
)

install(EXPORT mousetrap
    FILE ${PROJECT_NAME}ConfigExports.cmake
    NAMESPACE mousetrap::
    DESTINATION ${MOUSETRAP_CMAKECONFIG_INSTALL_DIR}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${MOUSETRAP_CMAKECONFIG_INSTALL_DIR}
)

### JULIA BINDING ###

option(MOUSETRAP_BUILD_JULIA_BINDING OFF)
if (MOUSETRAP_BUILD_JULIA_BINDING)

    find_package(JlCxx REQUIRED)
    get_target_property(JlCxx_location JlCxx::cxxwrap_julia LOCATION)
    get_filename_component(JlCxx_location ${JlCxx_location} DIRECTORY)

    add_library("${PROJECT_PREFIX}julia_binding" SHARED
        julia_binding/julia_binding.cpp
        julia_binding/julia_interface.cpp
    )

    target_link_libraries("${PROJECT_PREFIX}julia_binding" PUBLIC
        "$<BUILD_INTERFACE:mousetrap>"
        JlCxx::cxxwrap_julia
    )

    set(JULIA_BINDING_DIRECTORY "${CMAKE_SOURCE_DIR}/julia_binding")

    set_target_properties("${PROJECT_PREFIX}julia_binding" PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${JULIA_BINDING_DIRECTORY}"
    )

    add_custom_target("${PROJECT_PREFIX}julia_binding_test"
        COMMAND julia mousetrap.jl
        WORKING_DIRECTORY "${JULIA_BINDING_DIRECTORY}"
        DEPENDS mousetrap_julia_binding
    )
endif()

