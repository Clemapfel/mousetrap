cmake_minimum_required(VERSION 3.22)
project(mousetrap VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
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
    message(FATAL_ERROR "Missing Dependency: glm headers were not detected")
endif()

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

    include/range.hpp
    src/range.inl

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
    src/image.inl

    include/frame.hpp
    src/frame.inl

    include/pixel_buffer.hpp
    src/pixel_buffer.inl

    include/scrollbar.hpp
    src/scrollbar.inl

    include/adjustment.hpp
    src/adjustment.inl

    include/gestures.hpp

    include/flow_box.hpp
    src/flow_box.inl

    include/stack.hpp
    src/stack.inl

    include/scrolled_window.hpp
    src/scrolled_window.inl

    include/time.hpp
    src/time.inl

    include/window.hpp
    src/window.inl

    include/tree_view.hpp

    include/application.hpp
    src/application.inl

    include/aspect_frame.hpp
    src/aspect_frame.inl

    include/signal_emitter.hpp
    src/signal_emitter.inl

    include/image_display.hpp
    src/image_display.inl
        include/event_controller.hpp)

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
    ${GLU}
    ${GTK_LIBRARIES}
)

### EXECUTABLE ###

add_library(app STATIC
    app/global_state.hpp
    app/color_picker.hpp
    app/palette.hpp
    app/primary_secondary_color_swapper.hpp
)

target_link_libraries(app PRIVATE mousetrap)
set_target_properties(app PROPERTIES
    LINKER_LANGUAGE CXX
)

add_executable(debug main.cpp)
target_link_libraries(debug app mousetrap)

