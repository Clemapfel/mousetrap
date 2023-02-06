cmake_minimum_required(VERSION 3.22)
project(mousetrap VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_STANDARD 20)

# TODO
add_definitions(-w)
# TODO

### DEPENDENCIES ###

# OpenGL
find_library(OpenGL REQUIRED NAMES GL)
find_library(GLEW REQUIRED NAMES GLEW)

# GTK
find_package(PkgConfig)
pkg_check_modules(GTK gtk4)

if (NOT GTK_FOUND)
    message(FATAL_ERROR "Missing Dependency: unable to locate gtk4")
endif()

# GLM
include(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX("glm/glm.hpp" GLM_FOUND)
if(NOT GLM_FOUND)
    message(WARNING "Missing Dependency: glm headers were not detected")
endif()

# Boost
find_package(Boost REQUIRED COMPONENTS system iostreams system)

### CONFIGURE ###

set(RESOURCE_PATH "${CMAKE_SOURCE_DIR}/resources/")
configure_file(
    "${CMAKE_SOURCE_DIR}/src/get_resource_path.hpp.in"
    "${CMAKE_SOURCE_DIR}/include/get_resource_path.hpp"
    IMMEDIATE @ONLY
)

### LIBRARY ###

add_library(mousetrap SHARED
    include/get_resource_path.hpp

    include/gl_common.hpp
    src/gl_common.inl

    include/shape.hpp
    src/shape.inl

    include/colors.hpp
    src/colors.inl

    include/shader.hpp
    src/shader.inl

    include/gl_area.hpp
    src/gl_area.inl

    include/gl_transform.hpp
    src/transform.inl

    include/widget.hpp
    src/widget.inl

    include/angle.hpp
    src/angle.inl

    include/gtk_common.hpp
    src/gtk_common.inl

    include/geometry.hpp
    src/geometry.inl

    include/vector.hpp

    include/render_task.hpp
    src/render_task.inl

    include/scale.hpp
    src/scale.inl

    include/spin_button.hpp
    src/spin_button.inl

    include/entry.hpp
    src/entry.inl

    include/button.hpp
    src/button.inl

    include/box.hpp
    src/box.inl

    include/overlay.hpp
    src/overlay.inl

    include/toggle_button.hpp
    src/toggle_button.inl

    include/label.hpp
    src/label.inl

    include/texture.hpp
    src/texture.inl

    include/image.hpp
        src/image.cpp

    include/frame.hpp
    src/frame.inl

    include/pixel_buffer.hpp
    src/pixel_buffer.inl

    include/scrollbar.hpp
    src/scrollbar.inl

    include/adjustment.hpp
    src/adjustment.inl

    include/flow_box.hpp
    src/flow_box.inl

    include/stack.hpp
    src/stack.inl

    include/scrolled_window.hpp
    src/scrolled_window.inl

    include/stack.hpp
    src/stack.inl

    include/time.hpp
    src/time.inl

    include/window.hpp
    src/window.inl

    include/application.hpp
    src/application.inl

    include/aspect_frame.hpp
    src/aspect_frame.inl

    include/signal_emitter.hpp
    src/signal_emitter.inl

    include/image_display.hpp
    src/image_display.inl

    include/event_controller.hpp
    src/event_controller.inl

    include/separator_line.hpp
    src/separator_line.inl

    include/menu_button.hpp
    src/menu_button.inl

    include/menu_model.hpp
        src/menu_model.cpp

    include/list_view.hpp
    src/list_view.cpp

    include/column_view.hpp
    src/column_view.cpp

    include/popover.hpp
    src/popover.inl

    include/texture_object.hpp

    include/revealer.hpp
    src/revealer.inl

    include/detachable_box.hpp
    src/detachable_box.inl

    include/expander.hpp
    src/expander.inl

    include/paned.hpp
    src/paned.inl

    include/dropdown.hpp
    src/dropdown.inl
    src/dropdown.cpp

    include/check_button.hpp
    src/check_button.inl

    include/signal_component.hpp

    include/grid_view.hpp
    src/grid_view.cpp

    include/reorderable_list.hpp
    src/reorderable_list.cpp

    include/blend_mode.hpp
    src/blend_mode.cpp

    include/viewport.hpp
    src/viewport.inl

    include/center_box.hpp
    src/center_box.inl

    include/selection_model.hpp
    src/selection_model.inl

    include/dialog.hpp
    src/dialog.inl

    include/popover_menu.hpp
    src/popover_menu.inl

    include/popover_menu_bar.hpp
    src/popover_menu_bar.inl

    include/action_map.hpp
    src/action_map.inl

    include/shortcut_controller.hpp
    src/shortcut_controller.inl

    include/file_chooser.hpp
    src/file_chooser.inl

    include/file.hpp
    src/file.inl

    include/key_file.hpp
    src/key_file.inl

    include/switch.hpp
    src/switch.inl

    include/info_message.hpp
    src/info_message.inl

    include/fixed.hpp
    src/fixed.inl

    include/frame_clock.hpp
    src/frame_clock.inl

    include/shortcut_viewer.hpp
    src/shortcut_viewer.inl

    include/subimage.hpp

    include/level_bar.hpp
    src/level_bar.inl

    include/string_compression.hpp

    include/action.hpp
    src/action.cpp

    include/link_button.hpp
    src/link_button.inl

    include/render_texture.hpp
    src/render_texture.cpp
)

set_target_properties(mousetrap PROPERTIES
    LINKER_LANGUAGE CXX
    CXX_STANDARD 20
)

target_include_directories(mousetrap PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${GTK_INCLUDE_DIRS}
)

target_link_libraries(mousetrap PUBLIC
    ${OpenGL}
    ${GLEW}
    ${GTK_LIBRARIES}
    ${Boost_LIBRARIES}
)

### EXECUTABLE ###

add_library(app SHARED

    app/add_shortcut_action.hpp
    app/src/add_shortcut_action.cpp

    app/algorithms.hpp
    app/src/algorithms.cpp

    app/animation_preview.hpp
    app/src/animation_preview.cpp

    app/app_component.hpp

    app/brush.hpp
    app/src/brush.cpp

    app/brush_options.hpp
    app/src/brush_options.cpp

    app/bubble_log_area.hpp
    app/src/bubble_log_area.cpp

    app/canvas.hpp
    app/src/canvas.cpp

    app/color_picker.hpp
    app/src/color_picker.cpp

    app/color_preview.hpp
    app/src/color_preview.cpp

    app/color_swapper.hpp
    app/src/color_swapper.cpp

    app/color_transform.hpp
    app/src/color_transform.cpp

    app/file_chooser_dialog.hpp
    app/src/file_chooser_dialog.inl

    app/frame_view.hpp
    app/src/frame_view.cpp

        app/project_state.hpp
        app/src/project_state.cpp

    app/image_transform.hpp
    app/src/image_transform.cpp

    app/layer.hpp
    app/src/layer.cpp

    app/layer_view.hpp
    app/src/layer_view.cpp

    app/menubar.hpp
    app/src/menubar.cpp

    app/palette.hpp
    app/src/palette.cpp

    app/palette_view.hpp
    app/src/palette_view.cpp

    app/save_file.hpp
    app/src/save_file.cpp

    app/selection.hpp
    app/src/selection.cpp

    app/config_files.hpp
    app/src/config_files.cpp

    app/shortcut_information.hpp
    app/src/shortcut_information.cpp

    app/toolbox.hpp
    app/src/toolbox.cpp

    app/tools.hpp

    app/tooltip.hpp
    app/src/tooltip.cpp

    app/verbose_color_picker.hpp
    app/src/verbose_color_picker.cpp
        app/app_signals.hpp app/open_uri.hpp app/detect_platform.hpp app/resize_canvas_dialog.hpp app/scale_canvas_dialog.hpp app/src/scale_canvas_dialog.cpp app/src/resize_canvas_dialog.cpp app/canvas_export.hpp app/src/canvas_export.cpp app/color_transform_dialog.hpp app/src/color_transform_dialog.cpp)

target_link_libraries(app PRIVATE mousetrap)
set_target_properties(app PROPERTIES
    LINKER_LANGUAGE CXX
)

add_executable(debug main.cpp)
target_link_libraries(debug app mousetrap)

