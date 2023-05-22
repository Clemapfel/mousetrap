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
            DEPENDS docs/Doxyfile.in
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
        include/mousetrap/action.hpp
        include/mousetrap/adjustment.hpp
        include/mousetrap/alignment.hpp
        include/mousetrap/angle.hpp
        include/mousetrap/application.hpp
        include/mousetrap/aspect_frame.hpp
        include/mousetrap/blend_mode.hpp
        include/mousetrap/box.hpp
        include/mousetrap/button.hpp
        include/mousetrap/center_box.hpp
        include/mousetrap/check_button.hpp
        include/mousetrap/click_event_controller.hpp
        include/mousetrap/clipboard.hpp
        include/mousetrap/color.hpp
        include/mousetrap/column_view.hpp
        include/mousetrap/cursor_type.hpp
        include/mousetrap/drag_event_controller.hpp
        include/mousetrap/drop_down.hpp
        include/mousetrap/entry.hpp
        include/mousetrap/event_controller.hpp
        include/mousetrap/expander.hpp
        include/mousetrap/file_chooser.hpp
        include/mousetrap/file_descriptor.hpp
        include/mousetrap/file_monitor.hpp
        include/mousetrap/file_system.hpp
        include/mousetrap/fixed.hpp
        include/mousetrap/focus_event_controller.hpp
        include/mousetrap/frame.hpp
        include/mousetrap/frame_clock.hpp
        include/mousetrap/geometry.hpp
        include/mousetrap/gl_common.hpp
        include/mousetrap/gl_transform.hpp
        include/mousetrap/grid.hpp
        include/mousetrap/grid_view.hpp
        include/mousetrap/header_bar.hpp
        include/mousetrap/icon.hpp
        include/mousetrap/image.hpp
        include/mousetrap/image_display.hpp
        include/mousetrap/justify_mode.hpp
        include/mousetrap/key_event_controller.hpp
        include/mousetrap/key_file.hpp
        include/mousetrap/label.hpp
        include/mousetrap/level_bar.hpp
        include/mousetrap/list_view.hpp
        include/mousetrap/log.hpp
        include/mousetrap/long_press_event_controller.hpp
        include/mousetrap/menu_model.hpp
        include/mousetrap/menubar.hpp
        include/mousetrap/motion_event_controller.hpp
        include/mousetrap/msaa_render_texture.hpp
        include/mousetrap/music.hpp
        include/mousetrap/music.hpp
        include/mousetrap/notebook.hpp
        include/mousetrap/orientation.hpp
        include/mousetrap/overlay.hpp
        include/mousetrap/pan_event_controller.hpp
        include/mousetrap/paned.hpp
        include/mousetrap/pinch_zoom_event_controller.hpp
        include/mousetrap/popover.hpp
        include/mousetrap/popover_button.hpp
        include/mousetrap/progress_bar.hpp
        include/mousetrap/relative_position.hpp
        include/mousetrap/render_area.hpp
        include/mousetrap/render_task.hpp
        include/mousetrap/render_texture.hpp
        include/mousetrap/revealer.hpp
        include/mousetrap/rotate_event_controller.hpp
        include/mousetrap/scale.hpp
        include/mousetrap/scale_mode.hpp
        include/mousetrap/scroll_event_controller.hpp
        include/mousetrap/scrollbar.hpp
        include/mousetrap/scrolled_window.hpp
        include/mousetrap/selection_model.hpp
        include/mousetrap/separator.hpp
        include/mousetrap/shader.hpp
        include/mousetrap/shortcut_controller.hpp
        include/mousetrap/signal_component.hpp
        include/mousetrap/signal_emitter.hpp
        include/mousetrap/sound.hpp
        include/mousetrap/sound_buffer.hpp
        include/mousetrap/spin_button.hpp
        include/mousetrap/spinner.hpp
        include/mousetrap/stack.hpp
        include/mousetrap/stylus_event_controller.hpp
        include/mousetrap/swipe_event_controller.hpp
        include/mousetrap/switch.hpp
        include/mousetrap/text_view.hpp
        include/mousetrap/texture.hpp
        include/mousetrap/texture_object.hpp
        include/mousetrap/time.hpp
        include/mousetrap/toggle_button.hpp
        include/mousetrap/transition_type.hpp
        include/mousetrap/vector.hpp
        include/mousetrap/widget.hpp
        include/mousetrap/window.hpp
        include/mousetrap/wrap_mode.hpp
        include/mousetrap/shape.hpp
        include/mousetrap/inline/action.inl
        include/mousetrap/inline/clipboard.inl
        include/mousetrap/inline/column_view.inl
        include/mousetrap/inline/drop_down.inl
        include/mousetrap/inline/log.inl
        include/mousetrap/inline/scale.inl
        include/mousetrap/inline/signal_emitter.inl
        include/mousetrap/inline/spin_button.inl
        include/mousetrap/inline/widget.inl
)

set(MOUSETRAP_SOURCE_FILES
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
    src/color.cpp
    src/column_view.cpp
    src/drag_event_controller.cpp
    src/drop_down.cpp
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
    src/popover_button.cpp
    src/progress_bar.cpp
    src/render_area.cpp
    src/render_task.cpp
    src/render_texture.cpp
    src/revealer.cpp
    src/rotate_event_controller.cpp
    src/scale.cpp
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
    src/sound.cpp
    src/sound_buffer.cpp
    src/spin_button.cpp
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
    src/window.cpp
    src/action.cpp
    src/popover_menu.cpp
)

add_library(mousetrap SHARED
    ${MOUSETRAP_HEADER_FILES}
    ${MOUSETRAP_SOURCE_FILES}
)

target_include_directories(mousetrap PUBLIC
    "${CMAKE_SOURCE_DIR}/include"
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
            target_include_directories("${PROJECT_PREFIX}${TEST_PREFIX}${name}" PRIVATE "${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}")
            add_test(NAME "${PROJECT_PREFIX}${TEST_PREFIX}${name}" COMMAND "${PROJECT_PREFIX}${TEST_PREFIX}${name}")
        endfunction()

        declare_test(debug)
        declare_test(main)
    endif()
endif()

### INSTALL ###

include(cmake/install-rules.cmake)

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

