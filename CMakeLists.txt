cmake_minimum_required(VERSION 3.22)
project(mousetrap VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0") # TODO
set(CMAKE_CXX_STANDARD 20)

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
        src/gl_common.cpp

    include/shape.hpp
        src/shape.cpp

    include/colors.hpp
        src/colors.cpp

    include/shader.hpp
        src/shader.cpp

    include/gl_area.hpp
        src/gl_area.cpp

    include/gl_transform.hpp
        src/gl_transform.cpp

    include/widget.hpp
        src/widget.cpp

    include/angle.hpp
        src/angle.cpp

    include/gtk_common.hpp
        src/gtk_common.cpp

    include/geometry.hpp
        src/geometry.cpp

    include/vector.hpp

    include/render_task.hpp
        src/render_task.cpp

    include/scale.hpp
        src/scale.cpp

    include/spin_button.hpp
        src/spin_button.cpp

    include/entry.hpp
        src/entry.cpp

    include/button.hpp
        src/button.cpp

    include/box.hpp
        src/box.cpp

    include/overlay.hpp
        src/overlay.cpp

    include/toggle_button.hpp
        src/toggle_button.cpp

    include/label.hpp
        src/label.cpp

    include/texture.hpp
        src/texture.cpp

    include/image.hpp
        src/image.cpp

    include/frame.hpp
        src/frame.cpp

    include/scrollbar.hpp
        src/scrollbar.cpp

    include/adjustment.hpp
        src/adjustment.cpp

    include/flow_box.hpp
        src/flow_box.cpp

    include/stack.hpp
        src/stack.cpp

    include/scrolled_window.hpp
        src/scrolled_window.cpp

    include/stack.hpp
        src/stack.cpp

    include/time.hpp
        src/time.cpp

    include/window.hpp
        src/window.cpp

    include/application.hpp
        src/application.cpp

    include/aspect_frame.hpp
        src/aspect_frame.cpp

    include/signal_emitter.hpp
        src/signal_emitter.cpp

    include/image_display.hpp
        src/image_display.cpp

    include/event_controller.hpp
        src/event_controller.cpp

    include/separator_line.hpp
        src/separator_line.cpp

    include/menu_button.hpp
        src/menu_button.cpp

    include/menu_model.hpp
        src/menu_model.cpp

    include/list_view.hpp
    src/list_view.cpp

    include/column_view.hpp
    src/column_view.cpp

    include/popover.hpp
        src/popover.cpp

    include/texture_object.hpp

    include/revealer.hpp
        src/revealer.cpp

    include/detachable_box.hpp
        src/detachable_box.cpp

    include/expander.hpp
        src/expander.cpp

    include/paned.hpp
        src/paned.cpp

    include/dropdown.hpp
    src/dropdown.inl
    src/dropdown.cpp

    include/check_button.hpp
        src/check_button.cpp

    include/signal_component.hpp

    include/grid_view.hpp
    src/grid_view.cpp

    include/reorderable_list.hpp
    src/reorderable_list.cpp

    include/blend_mode.hpp
    src/blend_mode.cpp

    include/viewport.hpp
        src/viewport.cpp

    include/center_box.hpp
    src/center_box.inl

    include/selection_model.hpp
        src/selection_model.cpp

    include/dialog.hpp
        src/dialog.cpp

    include/popover_menu.hpp
        src/popover_menu.cpp

    include/popover_menu_bar.hpp
        src/popover_menu_bar.cpp

    include/action_map.hpp
        src/action_map.cpp

    include/shortcut_controller.hpp
        src/shortcut_controller.cpp

    include/file_chooser.hpp
        src/file_chooser.cpp

    include/file.hpp
        src/file.cpp

    include/key_file.hpp
        src/key_file.cpp

    include/switch.hpp
        src/switch.cpp

    include/info_message.hpp
        src/info_message.cpp

    include/fixed.hpp
        src/fixed.cpp

    include/frame_clock.hpp
        src/frame_clock.cpp

    include/shortcut_viewer.hpp
        src/shortcut_viewer.cpp

    include/level_bar.hpp
        src/level_bar.cpp

    include/string_compression.hpp

    include/action.hpp
    src/action.cpp

    include/link_button.hpp
        src/link_button.cpp

    include/render_texture.hpp
    src/render_texture.cpp

    include/msaa_texture.hpp
    src/msaa_texture.cpp

    include/clipboard.hpp
    src/clipboard.cpp
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

    app/file_chooser_dialog.hpp
    app/src/file_chooser_dialog.inl

    app/frame_view.hpp
    app/src/frame_view.cpp

        app/project_state.hpp
        app/src/project_state.cpp

        app/image_transform_dialog.hpp
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
        app/app_signals.hpp app/open_uri.hpp app/detect_platform.hpp app/resize_canvas_dialog.hpp app/scale_canvas_dialog.hpp app/src/scale_canvas_dialog.cpp app/src/resize_canvas_dialog.cpp app/canvas_export.hpp app/src/canvas_export.cpp app/color_transform_dialog.hpp app/src/color_transform_dialog.cpp app/apply_scope.hpp app/src/image_transform_dialog.cpp app/src/canvas_transparency_layer.cpp app/src/canvas_layer_layer.cpp app/src/canvas_onionskin_layer.cpp app/src/canvas_grid_layer.cpp app/src/canvas_symmetry_ruler_layer.cpp app/src/canvas_brush_shape_layer.cpp app/src/canvas_user_input_layer.cpp app/src/canvas_wireframe_layer.cpp  app/src/canvas_selection_layer.cpp app/src/canvas_control_bar.cpp app/log_box.hpp app/src/log_box.cpp app/src/canvas_gradient_layer.cpp app/src/canvas_tool_options.cpp app/draw_data.hpp)

target_link_libraries(app PRIVATE mousetrap)
set_target_properties(app PROPERTIES
    LINKER_LANGUAGE CXX
)

add_executable(debug main.cpp)
target_link_libraries(debug app mousetrap)

