cmake_minimum_required(VERSION 3.18)

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
#   MOUSETRAP_BUILD_TESTS: option
#       Whether to build tests
#
#   MOUSETRAP_BUILD_DOCUMENTATION: option
#       Whether to build the documentation.
#       Additional Dependencies when using this option:
#           Doxygen
#
#   MOUSETRAP_USE_OPENGL: boolean
#       If true, all OpenGL-related parts of mousetrap will not be available

### PROJECT ###

project(mousetrap
    VERSION 0.1.0
    LANGUAGES CXX
)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(PROJECT_PREFIX "")#"mousetrap_") # TODO

### DEPENDENCIES ###

if (NOT DEFINED MOUSETRAP_ENABLE_OPENGL_COMPONENT)
    set(MOUSETRAP_ENABLE_OPENGL_COMPONENT ON)
endif()
message("[mousetrap] OpenGL Component enabled: ${MOUSETRAP_ENABLE_OPENGL_COMPONENT}")
if (MOUSETRAP_ENABLE_OPENGL_COMPONENT)
    # OpenGL
    find_package(OpenGL)
    if (NOT ${OpenGL_FOUND}) # use library override
        find_library(OpenGL REQUIRED NAMES OpenGL GL opengl32 OPENGL32 OpenGL32)
    endif()

    # GLEW
    find_library(GLEW REQUIRED NAMES glew glew32 GLEW GLEW32)
endif()

configure_file(${CMAKE_SOURCE_DIR}/include/mousetrap/gl_common.hpp.in ${CMAKE_SOURCE_DIR}/include/mousetrap/gl_common.hpp @ONLY)

# GTK4
find_package(PkgConfig)
pkg_check_modules(GTK REQUIRED gtk4)

# Vector
include(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX(glm/glm.hpp GLM_FOUND)
if(NOT GLM_FOUND)
    message(FATAL_ERROR "[mousetrap] Missing Dependency: unable to locate GLM")
endif()

### MOUSETRAP ##

set(MOUSETRAP_HEADER_FILES
    include/mousetrap/action.hpp
    include/mousetrap/adjustment.hpp
    include/mousetrap/alignment.hpp
    include/mousetrap/angle.hpp
    include/mousetrap/application.hpp
    include/mousetrap/aspect_frame.hpp
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
    include/mousetrap/menu_bar.hpp
    include/mousetrap/motion_event_controller.hpp
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
    include/mousetrap/revealer.hpp
    include/mousetrap/rotate_event_controller.hpp
    include/mousetrap/scale.hpp
    include/mousetrap/scroll_event_controller.hpp
    include/mousetrap/scrollbar.hpp
    include/mousetrap/viewport.hpp
    include/mousetrap/selection_model.hpp
    include/mousetrap/separator.hpp
    include/mousetrap/shortcut_event_controller.hpp
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
    include/mousetrap/time.hpp
    include/mousetrap/toggle_button.hpp
    include/mousetrap/transition_type.hpp
    include/mousetrap/vector.hpp
    include/mousetrap/widget.hpp
    include/mousetrap/window.hpp
    include/mousetrap/gl_common.hpp
    include/mousetrap/inline/action.inl
    include/mousetrap/inline/clipboard.inl
    include/mousetrap/inline/column_view.inl
    include/mousetrap/inline/drop_down.inl
    include/mousetrap/inline/log.inl
    include/mousetrap/inline/scale.inl
    include/mousetrap/inline/signal_emitter.inl
    include/mousetrap/inline/spin_button.inl
    include/mousetrap/inline/widget.inl
    include/mousetrap/key_codes.hpp
    include/mousetrap/inline/file_chooser.inl
    include/mousetrap/color_chooser.hpp
    include/mousetrap/alert_dialog.hpp
)

set(MOUSETRAP_SOURCE_FILES
    src/adjustment.cpp
    src/angle.cpp
    src/application.cpp
    src/aspect_frame.cpp
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
    src/revealer.cpp
    src/rotate_event_controller.cpp
    src/scale.cpp
    src/scroll_event_controller.cpp
    src/scrollbar.cpp
    src/viewport.cpp
    src/selection_model.cpp
    src/separator.cpp
    src/shortcut_event_controller.cpp
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
    src/time.cpp
    src/toggle_button.cpp
    src/widget.cpp
    src/window.cpp
    src/action.cpp
    src/popover_menu.cpp
    src/color_chooser.cpp
    src/gtk_common.cpp
    src/alert_dialog.cpp
)

if (MOUSETRAP_ENABLE_OPENGL_COMPONENT)

    set(MOUSETRAP_OPENGL_HEADER_FILES
            include/mousetrap/blend_mode.hpp
            include/mousetrap/shape.hpp
            include/mousetrap/gl_transform.hpp
            include/mousetrap/msaa_render_texture.hpp
            include/mousetrap/render_area.hpp
            include/mousetrap/render_task.hpp
            include/mousetrap/render_texture.hpp
            include/mousetrap/texture.hpp
            include/mousetrap/texture_object.hpp
            include/mousetrap/shader.hpp
            include/mousetrap/texture_scale_mode.hpp
            include/mousetrap/texture_wrap_mode.hpp
        )
    set(MOUSETRAP_HEADER_FILES "${MOUSETRAP_HEADER_FILES};${MOUSETRAP_OPENGL_HEADER_FILES}")

    set(MOUSETRAP_OPENGL_SOURCE_FILES
            src/blend_mode.cpp
            src/gl_common.cpp
            src/gl_transform.cpp
            src/msaa_render_texture.cpp
            src/render_area.cpp
            src/render_task.cpp
            src/render_texture.cpp
            src/shader.cpp
            src/texture.cpp
            src/shape.cpp
        )
    set(MOUSETRAP_SOURCE_FILES "${MOUSETRAP_SOURCE_FILES};${MOUSETRAP_OPENGL_SOURCE_FILES}")
endif()

add_library(mousetrap SHARED
    ${MOUSETRAP_HEADER_FILES}
    ${MOUSETRAP_SOURCE_FILES}
)

target_compile_features(mousetrap PUBLIC
    cxx_std_17
)

target_include_directories(mousetrap PUBLIC
    "${CMAKE_SOURCE_DIR}/include"
    "${GTK_INCLUDE_DIRS}"
)

if (MOUSETRAP_ENABLED_OPENGL_COMPONENT)

    target_link_libraries(mousetrap PUBLIC
        ${GTK_LIBRARIES}
    )
    set_target_properties(mousetrap PROPERTIES
        LINKER_LANGUAGE CXX
        POSITION_INDEPENDENT_CODE ON
        INTERFACE_INCLUDE_DIRECTORIES "${GTK_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${GTK_LIBRARIES}"
    )

else()

    target_link_libraries(mousetrap PUBLIC
        ${GTK_LIBRARIES}
        ${OpenGL}
        ${GLEW}
    )
    set_target_properties(mousetrap PROPERTIES
        LINKER_LANGUAGE CXX
        POSITION_INDEPENDENT_CODE ON
        INTERFACE_INCLUDE_DIRECTORIES "${GTK_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${OpenGL};${GLEW};${GTK_LIBRARIES}"
    )
endif()


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

