cmake_minimum_required(VERSION 3.12)

project(mousetrap
    VERSION 0.1.0
    DESCRIPTION "TODO"
    LANGUAGES CXX
)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

### DOCUMENTATION ###

option(BUILD_DOCUMENTATION OFF)

if (${BUILD_DOCUMENTATION})
    find_package(Doxygen REQUIRED)
    set(PROJECT_DOCUMENTATION_DIR "${CMAKE_SOURCE_DIR}/docs")
    if (DOXYGEN_FOUND AND EXISTS "${PROJECT_DOCUMENTATION_DIR}")
        configure_file(
            "${PROJECT_DOCUMENTATION_DIR}/Doxyfile.in"
            "${PROJECT_DOCUMENTATION_DIR}/Doxyfile"
            @ONLY
        )
        add_custom_target(build_docs
            COMMAND doxygen
            WORKING_DIRECTORY "${PROJECT_DOCUMENTATION_DIR}"
        )
    endif()
endif()

### DEPENDENCIES ###

# OpenGL
find_library(OpenGL REQUIRED NAMES GL)

# GLEW
find_library(GLEW REQUIRED NAMES GLEW)

# SFML (audio)
find_package(SFML COMPONENTS audio REQUIRED)

# GTK4
find_package(PkgConfig)
pkg_check_modules(GTK gtk4)

if (NOT GTK_FOUND)
    message(FATAL_ERROR "Missing Dependency: unable to locate gtk4")
endif()

# OpenGL Math
include(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX(glm/glm.hpp GLM_FOUND)
if(NOT GLM_FOUND)
    message(FATAL_ERROR "Missing Dependency: unable to locate glm")
endif()

### MOUSETRAP ##

set(MOUSETRAP_HEADER_FILES
    include/shape.hpp
    include/gl_common.hpp
    include/vector.hpp
    include/shader.hpp
    include/color.hpp
    include/gl_transform.hpp
    include/angle.hpp
    include/texture.hpp
    include/image.hpp
    include/geometry.hpp
    include/render_texture.hpp
    include/render_task.hpp
    include/blend_mode.hpp
    include/texture_object.hpp
    include/scale_mode.hpp
    include/wrap_mode.hpp
    include/msaa_render_texture.hpp
    include/alignment.hpp
    include/orientation.hpp
    include/sound_buffer.hpp
    include/sound.hpp
    include/time.hpp
    include/music.hpp
    include/signal_emitter.hpp
    src/signal_emitter.inl
    include/signal_component.hpp
    include/application.hpp
    include/window.hpp
    include/widget.hpp
    src/widget.inl
    include/cursor_type.hpp
    include/action.hpp
    src/action.inl
    include/frame_clock.hpp
    include/event_controller.hpp
    include/key_event_controller.hpp
    include/motion_event_controller.hpp
    include/aspect_frame.hpp
    include/box.hpp
    include/button.hpp
    include/check_button.hpp
    include/toggle_button.hpp
    include/entry.hpp
    include/expander.hpp
    include/revealer.hpp
    include/frame.hpp
    include/image_display.hpp
    include/key_file.hpp
    include/label.hpp
    include/justify_mode.hpp
    include/text_view.hpp
    include/list_view.hpp
    include/selection_model.hpp
    include/render_area.hpp
    include/click_event_controller.hpp
    include/scroll_event_controller.hpp
    include/focus_event_controller.hpp
    include/drag_event_controller.hpp
    include/pinch_zoom_event_controller.hpp
    include/rotate_event_controller.hpp
    include/adjustment.hpp
    include/scrollbar.hpp
    include/viewport.hpp
    include/scrolled_window.hpp
    include/grid_view.hpp
    include/menu_model.hpp
    include/popover_menu_button.hpp
    include/popover.hpp
    include/relative_position.hpp
    include/center_box.hpp
    include/clipboard.hpp
    include/stack.hpp
    include/transition_type.hpp
    include/notebook.hpp
    include/column_view.hpp
    include/spin_button.hpp
    include/drop_down.hpp
    include/fixed.hpp
    include/level_bar.hpp
    include/spinner.hpp
    include/progress_bar.hpp
    include/paned.hpp
    include/scale.hpp
    include/overlay.hpp
    include/separator.hpp
    include/switch.hpp
    include/file_system.hpp
    include/file_descriptor.hpp
    include/file_monitor.hpp
    include/orientable.hpp
    include/grid.hpp
    include/selectable.hpp
    include/music.hpp
    include/icon.hpp
    include/log.hpp
    include/file_chooser.hpp
    include/shortcut_controller.hpp
    include/menubar.hpp
    include/long_press_event_controller.hpp
    include/pan_event_controller.hpp
    include/swipe_event_controller.hpp
    include/stylus_event_controller.hpp
    include/header_bar.hpp
)

set(MOUSETRAP_SOURCE_FILES
    src/shape.cpp
    src/gl_common.cpp
    src/shader.cpp
    src/color.cpp
    src/gl_transform.cpp
    src/angle.cpp
    src/texture.cpp
    src/image.cpp
    src/geometry.cpp
    src/render_texture.cpp
    src/render_task.cpp
    src/blend_mode.cpp
    src/msaa_render_texture.cpp
    src/sound_buffer.cpp
    src/sound.cpp
    src/time.cpp
    src/music.cpp
    src/signal_emitter.cpp
    src/application.cpp
    src/window.cpp
    src/widget.cpp
    src/action.cpp
    src/frame_clock.cpp
    src/event_controller.cpp
    src/key_event_controller.cpp
    src/motion_event_controller.cpp
    src/aspect_frame.cpp
    src/box.cpp
    src/button.cpp
    src/check_button.cpp
    src/toggle_button.cpp
    src/entry.cpp
    src/expander.cpp
    src/revealer.cpp
    src/frame.cpp
    src/image_display.cpp
    src/key_file.cpp
    src/label.cpp
    src/text_view.cpp
    src/list_view.cpp
    src/selection_model.cpp
    src/click_event_controller.cpp
    src/scroll_event_controller.cpp
    src/focus_event_controller.cpp
    src/drag_event_controller.cpp
    src/pinch_zoom_event_controller.cpp
    src/rotate_event_controller.cpp
    src/adjustment.cpp
    src/scrollbar.cpp
    src/viewport.cpp
    src/scrolled_window.cpp
    src/grid_view.cpp
    src/menu_model.cpp
    src/popover_menu_button.cpp
    src/popover.cpp
    src/render_area.cpp
    src/center_box.cpp
    src/clipboard.cpp
    src/stack.cpp
    src/notebook.cpp
    src/column_view.cpp
    src/column_view.inl
    src/spin_button.cpp
    src/drop_down.cpp
    src/fixed.cpp
    src/level_bar.cpp
    src/spinner.cpp
    src/progress_bar.cpp
    src/paned.cpp
    src/scale.cpp
    src/overlay.cpp
    src/separator.cpp
    src/switch.cpp
    src/file_system.cpp
    src/file_descriptor.cpp
    src/file_monitor.cpp
    src/grid.cpp
    src/music.cpp
    src/icon.cpp
    src/log.cpp
    src/file_chooser.cpp
    src/file_chooser.cpp
    src/shortcut_controller.cpp
    src/signal_component.cpp
    src/menu_bar.cpp
    src/long_press_event_controller.cpp
    src/pan_event_controller.cpp
    src/swipe_event_controller.cpp
    src/stylus_event_controller.cpp
    src/header_bar.cpp
        src/log.inl
        src/spin_button.inl
        src/scale.inl
        src/clipboard.inl
        src/drop_down.inl
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
)


### INSTALL ###

include("${CMAKE_SOURCE_DIR}/cmake/install-rules.cmake")