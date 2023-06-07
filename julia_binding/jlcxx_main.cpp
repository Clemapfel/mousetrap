#include "mousetrap.cpp"

// ### MAIN

JLCXX_MODULE define_julia_module(jlcxx::Module& module)
{
    module.set_const("GTK_MAJOR_VERSION", (int) GTK_MAJOR_VERSION);
    module.set_const("GTK_MINOR_VERSION", (int) GTK_MINOR_VERSION);

    implement_orientation(module);
    implement_time(module);

    implement_adjustment(module);
    implement_action(module);
    implement_alignment(module);
    implement_application(module);
    implement_window(module);

    implement_event_controller(module);
    implement_drag_event_controller(module);
    implement_click_event_controller(module);
    implement_focus_event_controller(module);
    implement_key_event_controller(module);
    implement_long_press_event_controller(module);
    implement_motion_event_controller(module);
    implement_pinch_zoom_event_controller(module);
    implement_rotate_event_controller(module);
    implement_scroll_event_controller(module);
    implement_shortcut_event_controller(module);
    implement_stylus_event_controller(module);
    implement_swipe_event_controller(module);
    implement_pan_event_controller(module);

    implement_color(module);
    implement_icon(module);
    implement_image(module);
    implement_cursor_type(module);

    implement_relative_position(module);
    implement_menu_model(module);
    implement_menu_bar(module);
    implement_popover_menu(module);
    implement_popover(module);
    implement_popover_button(module);
    implement_progress_bar(module);

    implement_aspect_frame(module);
    implement_blend_mode(module);
    implement_box(module);
    implement_button(module);
    implement_center_box(module);
    implement_check_button(module);
    implement_clipboard(module);

    implement_selection_model(module);
    implement_list_view(module);
    implement_grid_view(module);
    implement_column_view(module);
    implement_grid(module);

    implement_image_display(module);
    implement_drop_down(module);
    implement_entry(module);
    implement_expander(module);
    implement_file_chooser(module);
    implement_file_descriptor(module);
    implement_file_monitor(module);
    implement_file_system(module);
    implement_fixed(module);
    implement_frame(module);
    implement_frame_clock(module);
    implement_geometry(module);
    implement_gl_common(module);
    implement_gl_transform(module);
    implement_header_bar(module);
    implement_key_file(module);

    implement_justify_mode(module);
    implement_label(module);
    implement_text_view(module);

    implement_level_bar(module);
    implement_log(module);

    implement_msaa_render_texture(module);
    implement_music(module);
    implement_notebook(module);
    implement_overlay(module);
    implement_paned(module);

    implement_render_area(module);
    implement_render_task(module);
    implement_render_texture(module);
    implement_revealer(module);

    implement_scale(module);
    implement_scale_mode(module);

    implement_scrollbar(module);
    implement_separator(module);
    implement_shader(module);
    implement_shape(module);

    implement_sound(module);
    implement_sound_buffer(module);
    implement_spin_button(module);
    implement_spinner(module);
    implement_stack(module);

    implement_switch(module);
    implement_texture(module);
    implement_toggle_button(module);
    implement_transition_type(module);
    implement_viewport(module);
    implement_widget(module);
}