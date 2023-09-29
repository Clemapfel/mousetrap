project('mousetrap', 'cpp',
    version: '0.3.0',
    default_options : ['cpp_std=c++17']
)
add_project_arguments([
    '-Wno-unused-variable',
    '-Wno-unused-function',
    '-Wno-unused-but-set-variable',
], language: 'cpp')

### DEPENDENCIES ###

OPENGL = dependency(['gl', 'opengl', 'opengl32'],
    required: false
)

GLEW = dependency(['glew', 'glew32'],
    required: false
)

if not GLEW.found()
    GLEW = meson.get_compiler('cpp').find_library('glew', required: false)
    if not GLEW.found()
        GLEW = meson.get_compiler('cpp').find_library('glew32', required: false)
        if not GLEW.found() and OPENGL.found()
            error('Missing dependency: glew')
        endif
    endif
endif

GTK4 = dependency(['gtk4', 'gtk-4.0'],
    required: true,
    version: '>=4.8'
)

ADWAITA = dependency('libadwaita-1',
    required: true,
    version: '>=1.2'
)

if not meson.get_compiler('cpp').has_header('glm/glm.hpp')
   error('Could not find GLM (OpenGL Mathematics), `glm/glm.hpp` missing.')
endif

### MOUSETRAP ###

# configure gl_common.hpp.in, then move it to include/mousetrap
subdir('include/mousetrap')
error_maybe = run_command('cp', meson.build_root() + '/include/mousetrap/gl_common.hpp', meson.source_root() + '/include/mousetrap', check: false).stderr().strip()
if error_maybe != ''
    error(error_maybe)
endif

MOUSETRAP_HEADER_FILES = [
    'include/mousetrap/action_bar.hpp',
    'include/mousetrap/action.hpp',
    'include/mousetrap/adjustment.hpp',
    'include/mousetrap/alert_dialog.hpp',
    'include/mousetrap/alignment.hpp',
    'include/mousetrap/angle.hpp',
    'include/mousetrap/animation.hpp',
    'include/mousetrap/application.hpp',
    'include/mousetrap/aspect_frame.hpp',
    'include/mousetrap/blend_mode.hpp',
    'include/mousetrap/box.hpp',
    'include/mousetrap/button.hpp',
    'include/mousetrap/center_box.hpp',
    'include/mousetrap/check_button.hpp',
    'include/mousetrap/clamp_frame.hpp',
    'include/mousetrap/click_event_controller.hpp',
    'include/mousetrap/clipboard.hpp',
    'include/mousetrap/color_chooser.hpp',
    'include/mousetrap/color.hpp',
    'include/mousetrap/column_view.hpp',
    'include/mousetrap/cursor_type.hpp',
    'include/mousetrap/drag_event_controller.hpp',
    'include/mousetrap/drop_down.hpp',
    'include/mousetrap/entry.hpp',
    'include/mousetrap/event_controller.hpp',
    'include/mousetrap/expander.hpp',
    'include/mousetrap/file_chooser.hpp',
    'include/mousetrap/file_descriptor.hpp',
    'include/mousetrap/file_monitor.hpp',
    'include/mousetrap/file_system.hpp',
    'include/mousetrap/fixed.hpp',
    'include/mousetrap/flow_box.hpp',
    'include/mousetrap/focus_event_controller.hpp',
    'include/mousetrap/frame_clock.hpp',
    'include/mousetrap/frame.hpp',
    'include/mousetrap/geometry.hpp',
    'include/mousetrap/gl_common.hpp',
    'include/mousetrap/gl_transform.hpp',
    'include/mousetrap/grid.hpp',
    'include/mousetrap/grid_view.hpp',
    'include/mousetrap/gtk_common.hpp',
    'include/mousetrap/header_bar.hpp',
    'include/mousetrap/icon.hpp',
    'include/mousetrap/image_display.hpp',
    'include/mousetrap/image.hpp',
    'include/mousetrap/justify_mode.hpp',
    'include/mousetrap/key_codes.hpp',
    'include/mousetrap/key_event_controller.hpp',
    'include/mousetrap/key_file.hpp',
    'include/mousetrap/label.hpp',
    'include/mousetrap/level_bar.hpp',
    'include/mousetrap/list_view.hpp',
    'include/mousetrap/log.hpp',
    'include/mousetrap/long_press_event_controller.hpp',
    'include/mousetrap/menu_bar.hpp',
    'include/mousetrap/menu_model.hpp',
    'include/mousetrap/motion_event_controller.hpp',
    'include/mousetrap/msaa_render_texture.hpp',
    'include/mousetrap/notebook.hpp',
    'include/mousetrap/orientation.hpp',
    'include/mousetrap/overlay.hpp',
    'include/mousetrap/paned.hpp',
    'include/mousetrap/pan_event_controller.hpp',
    'include/mousetrap/pinch_zoom_event_controller.hpp',
    'include/mousetrap/popover_button.hpp',
    'include/mousetrap/popover.hpp',
    'include/mousetrap/popover_menu.hpp',
    'include/mousetrap/popup_message.hpp',
    'include/mousetrap/progress_bar.hpp',
    'include/mousetrap/relative_position.hpp',
    'include/mousetrap/render_area.hpp',
    'include/mousetrap/render_task.hpp',
    'include/mousetrap/render_texture.hpp',
    'include/mousetrap/revealer.hpp',
    'include/mousetrap/rotate_event_controller.hpp',
    'include/mousetrap/scale.hpp',
    'include/mousetrap/scrollbar.hpp',
    'include/mousetrap/scroll_event_controller.hpp',
    'include/mousetrap/selection_model.hpp',
    'include/mousetrap/separator.hpp',
    'include/mousetrap/shader.hpp',
    'include/mousetrap/shape.hpp',
    'include/mousetrap/shortcut_event_controller.hpp',
    'include/mousetrap/signal_component.hpp',
    'include/mousetrap/signal_emitter.hpp',
    'include/mousetrap/spin_button.hpp',
    'include/mousetrap/spinner.hpp',
    'include/mousetrap/stack.hpp',
    'include/mousetrap/style_manager.hpp',
    'include/mousetrap/stylus_event_controller.hpp',
    'include/mousetrap/swipe_event_controller.hpp',
    'include/mousetrap/switch.hpp',
    'include/mousetrap/texture.hpp',
    'include/mousetrap/texture_object.hpp',
    'include/mousetrap/texture_scale_mode.hpp',
    'include/mousetrap/texture_wrap_mode.hpp',
    'include/mousetrap/text_view.hpp',
    'include/mousetrap/theme.hpp',
    'include/mousetrap/time.hpp',
    'include/mousetrap/toggle_button.hpp',
    'include/mousetrap/transform_bin.hpp',
    'include/mousetrap/transition_type.hpp',
    'include/mousetrap/vector.hpp',
    'include/mousetrap/viewport.hpp',
    'include/mousetrap/widget.hpp',
    'include/mousetrap/window.hpp',
    'include/mousetrap/inline/action.hpp',
    'include/mousetrap/inline/clipboard.hpp',
    'include/mousetrap/inline/column_view.hpp',
    'include/mousetrap/inline/drop_down.hpp',
    'include/mousetrap/inline/file_chooser.hpp',
    'include/mousetrap/inline/file_monitor.hpp',
    'include/mousetrap/inline/log.hpp',
    'include/mousetrap/inline/scale.hpp',
    'include/mousetrap/inline/signal_emitter.hpp',
    'include/mousetrap/inline/spin_button.hpp',
    'include/mousetrap/inline/widget.hpp'
]

MOUSETRAP_SOURCE_FILES = [
    'src/action_bar.cpp',
    'src/action.cpp',
    'src/adjustment.cpp',
    'src/alert_dialog.cpp',
    'src/angle.cpp',
    'src/animation.cpp',
    'src/application.cpp',
    'src/aspect_frame.cpp',
    'src/blend_mode.cpp',
    'src/box.cpp',
    'src/button.cpp',
    'src/center_box.cpp',
    'src/check_button.cpp',
    'src/clamp_frame.cpp',
    'src/click_event_controller.cpp',
    'src/clipboard.cpp',
    'src/color_chooser.cpp',
    'src/color.cpp',
    'src/column_view.cpp',
    'src/drag_event_controller.cpp',
    'src/drop_down.cpp',
    'src/entry.cpp',
    'src/event_controller.cpp',
    'src/expander.cpp',
    'src/file_chooser.cpp',
    'src/file_descriptor.cpp',
    'src/file_monitor.cpp',
    'src/file_system.cpp',
    'src/fixed.cpp',
    'src/flow_box.cpp',
    'src/focus_event_controller.cpp',
    'src/frame_clock.cpp',
    'src/frame.cpp',
    'src/geometry.cpp',
    'src/gl_common.cpp',
    'src/gl_transform.cpp',
    'src/grid.cpp',
    'src/grid_view.cpp',
    'src/gtk_common.cpp',
    'src/header_bar.cpp',
    'src/icon.cpp',
    'src/image.cpp',
    'src/image_display.cpp',
    'src/key_event_controller.cpp',
    'src/key_file.cpp',
    'src/label.cpp',
    'src/level_bar.cpp',
    'src/list_view.cpp',
    'src/log.cpp',
    'src/long_press_event_controller.cpp',
    'src/menu_bar.cpp',
    'src/menu_model.cpp',
    'src/motion_event_controller.cpp',
    'src/msaa_render_texture.cpp',
    'src/notebook.cpp',
    'src/overlay.cpp',
    'src/paned.cpp',
    'src/pan_event_controller.cpp',
    'src/pinch_zoom_event_controller.cpp',
    'src/popover_button.cpp',
    'src/popover.cpp',
    'src/popover_menu.cpp',
    'src/popup_message.cpp',
    'src/progress_bar.cpp',
    'src/render_area.cpp',
    'src/render_task.cpp',
    'src/render_texture.cpp',
    'src/revealer.cpp',
    'src/rotate_event_controller.cpp',
    'src/scale.cpp',
    'src/scrollbar.cpp',
    'src/scroll_event_controller.cpp',
    'src/selection_model.cpp',
    'src/separator.cpp',
    'src/shader.cpp',
    'src/shape.cpp',
    'src/shortcut_event_controller.cpp',
    'src/signal_component.cpp',
    'src/signal_emitter.cpp',
    'src/spin_button.cpp',
    'src/spinner.cpp',
    'src/stack.cpp',
    'src/style_manager.cpp',
    'src/stylus_event_controller.cpp',
    'src/swipe_event_controller.cpp',
    'src/switch.cpp',
    'src/texture.cpp',
    'src/text_view.cpp',
    'src/time.cpp',
    'src/toggle_button.cpp',
    'src/transform_bin.cpp',
    'src/viewport.cpp',
    'src/widget.cpp',
    'src/window.cpp'
]

MOUSETRAP_LIBRARY = library('mousetrap',
    sources: [MOUSETRAP_HEADER_FILES, MOUSETRAP_SOURCE_FILES],
    dependencies: [OPENGL, GLEW, ADWAITA],
    version: '0.3.0',
    include_directories: ['include'],
    install: true
)
install_headers('mousetrap.hpp')
install_subdir('include/mousetrap', install_dir: 'include')

if get_option('MOUSETRAP_BUILD_TESTS')
    MOUSETRAP_TEST = executable('test_main',
        sources: 'test/main.cpp',
        dependencies: [OPENGL, GLEW, ADWAITA],
        include_directories: ['include'],
        link_with: MOUSETRAP_LIBRARY,
        install: false
    )
endif

if get_option('MOUSETRAP_BUILD_DOCUMENTATION')
    message('Building Docs...')
    run_command('doxygen', 'docs', check: true)
    message('Done.')
endif