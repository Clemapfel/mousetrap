// 
// Copyright 2022 Clemens Cords
// Created on 9/25/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/scale.hpp>
#include <include/toggle_button.hpp>
#include <include/label.hpp>
#include <include/box.hpp>
#include <include/gl_area.hpp>
#include <include/time.hpp>

#include <app/global_state.hpp>

namespace mousetrap
{
    class Preview : public AppComponent
    {
        public:
            Preview();
            ~Preview();

            operator Widget*() override;
            void update() override;

            // zoom level
            // pause / unpause
            // fps slider

        private:
            GLArea _transparency_tiling_area;
            GLArea _layer_area;

            static void on_layer_area_realize(Widget*, Preview*);
            static void on_transparency_area_realize(Widget*, Preview*);

            static void on_gl_area_resize(GLArea*, int, int, Preview*);

            Shape* _transparency_tiling_shape;
            Shader _transparency_tiling_shader;

            Vector2f _canvas_size;
            AspectFrame _gl_area_frame = AspectFrame(1);
            Overlay _gl_area_overlay;

            std::vector<Shape*> _layer_shapes;

            ToggleButton _play_pause_button;
            Label _play_pause_label = Label("<tt>&#9654;</tt>");
            static void on_play_pause_button_toggled(ToggleButton*, Preview*);

            MenuButton _menu_button;
            Label _menu_button_label = Label("&#9881;");

            Popover _menu_popover;
            Box _menu_popover_box = Box(GTK_ORIENTATION_VERTICAL, state::margin_unit);

            size_t _fps = 8;
            size_t _scale = 2;
            bool _is_unpaused = false;
            bool _frame_i_visible = true;

            size_t _current_frame = 0;
            size_t _current_frame_sum = 0; // in microseconds
            void set_frame(size_t);

            Label _frame_label = Label("00");

            Label _fps_label = Label("FPS: ");
            SpinButton _fps_spin_button = SpinButton(1, 240, 1);
            Box _fps_box = Box(GTK_ORIENTATION_HORIZONTAL);
            SeparatorLine _fps_separator;
            static void on_fps_spin_button_value_changed(SpinButton*, Preview*);

            Label _scale_label = Label("Scale: ");
            SpinButton _scale_spin_button = SpinButton(1, 16, 1);
            Box _scale_box = Box(GTK_ORIENTATION_HORIZONTAL);
            SeparatorLine _scale_separator;
            static void on_scale_spin_button_value_changed(SpinButton*, Preview*);

            Label _show_frame_i_label = Label("Show Frame: ");
            CheckButton _show_frame_i_check_button;
            Box _show_frame_i_box = Box(GTK_ORIENTATION_HORIZONTAL);
            SeparatorLine _show_frame_i_separator;
            static void on_show_frame_i_label_button_toggled(CheckButton*, Preview*);

            Box _toolbox_box = Box(GTK_ORIENTATION_HORIZONTAL);
            SeparatorLine _toolbox_separator;
            Revealer _toolbox_revealer;

            MotionEventController _motion_event_controller;
            static void on_motion_enter(MotionEventController*, double x, double y, Preview* instance);
            static void on_motion_leave(MotionEventController*, Preview* instance);

            KeyEventController _key_event_controller;
            bool _key_bindings_active = false; // cursor in bounds
            static bool on_key_pressed(KeyEventController*, guint keyval, guint keycode, GdkModifierType state, Preview* instance);

            Overlay _main;

            static gboolean on_tick_callback(GtkWidget* widget, GdkFrameClock* frame_clock, Preview* instance);
    };
}

namespace mousetrap
{
    Preview::~Preview()
    {
        delete _transparency_tiling_shape;
    }

    void Preview::on_transparency_area_realize(Widget* widget, Preview* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        instance->_transparency_tiling_shape = new Shape();
        instance->_transparency_tiling_shape->as_rectangle({0, 0}, {1, 1});

        instance->_transparency_tiling_shader = Shader();
        instance->_transparency_tiling_shader.create_from_file(get_resource_path() + "shaders/transparency_tiling.frag", ShaderType::FRAGMENT);

        instance->_transparency_tiling_area.make_current();
        instance->_transparency_tiling_area.clear_render_tasks();

        auto task = RenderTask( instance->_transparency_tiling_shape, & instance->_transparency_tiling_shader);
        task.register_vec2("_canvas_size", & instance->_canvas_size);
        instance->_transparency_tiling_area.add_render_task(task);

        instance->_transparency_tiling_area.queue_render();
    }

    void Preview::on_layer_area_realize(Widget* widget, Preview* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();
        instance->update();
        area->queue_render();
    }

    void Preview::on_gl_area_resize(GLArea* area, int w, int h, Preview* instance)
    {
        instance->_canvas_size = {w, h};
        area->queue_render();
    }

    void Preview::on_play_pause_button_toggled(ToggleButton* button, Preview* instance)
    {
        instance->_is_unpaused = button->get_active();

        // TODO: reset frame to one being edited
    }

    void Preview::on_fps_spin_button_value_changed(SpinButton* scale, Preview* instance)
    {
        float value = scale->get_value();
        instance->_fps = value;
    }

    void Preview::on_scale_spin_button_value_changed(SpinButton* scale, Preview* instance)
    {
        float value = scale->get_value();
        instance->_layer_area.set_size_request(state::layer_resolution * Vector2ui(value));
        instance->_transparency_tiling_area.set_size_request(state::layer_resolution * Vector2ui(value));

        instance->_scale = value;
    }

    void Preview::on_show_frame_i_label_button_toggled(CheckButton* button, Preview* instance)
    {
        instance->_frame_i_visible = button->get_is_checked();
        instance->_frame_label.set_visible(instance->_frame_i_visible);
    }

    void Preview::on_motion_enter(MotionEventController*, double x, double y, Preview* instance)
    {
        instance->_main.grab_focus();
        instance->_toolbox_revealer.set_revealed(true);
    }

    void Preview::on_motion_leave(MotionEventController*, Preview* instance)
    {
        if (not instance->_menu_popover.get_visible())
            instance->_toolbox_revealer.set_revealed(false);
    }

    bool Preview::on_key_pressed(KeyEventController* self, guint keyval, guint keycode, GdkModifierType state, Preview* instance)
    {
        GdkEvent* event = self->get_current_event();
        throw std::invalid_argument("TODO");
        return FALSE;
    }

    gboolean Preview::on_tick_callback(GtkWidget* widget, GdkFrameClock* frame_clock, Preview* instance)
    {
        static auto previous = gdk_frame_clock_get_frame_time(frame_clock);
        auto current = gdk_frame_clock_get_frame_time(frame_clock);

        if (instance->_is_unpaused)
        {
            instance->_current_frame_sum += (current - previous);

            auto step = seconds(1.f / instance->_fps).as_microseconds();
            size_t next_frame = instance->_current_frame;
            while (instance->_current_frame_sum - step >= 0)
            {
                next_frame += 1;
                if (next_frame == state::n_frames)
                    next_frame = 0;

                instance->_current_frame_sum -= step;
            }

            instance->set_frame(next_frame);
        }

        instance->_layer_area.queue_render();
        previous = current;

        return true;
    }

    void Preview::set_frame(size_t i)
    {
        assert(i < state::n_frames);

        _current_frame = i;

        assert(_layer_shapes.size() == state::layers.size());
        for (size_t i = 0; i < _layer_shapes.size(); ++i)
        {
            auto& frame = state::layers.at(i)->frames.at(_current_frame);
            _layer_shapes.at(i)->set_texture(frame.texture);
        }

        std::stringstream label_text;
        label_text << "<span bgcolor=\"#00000060\" fgcolor=\"#FFFFFF\">"
                   << ((_current_frame < 10) ? "0" : "") + std::to_string(_current_frame)
                   << "</span>";

        _frame_label.set_text(label_text.str());
    }

    Preview::Preview()
    {
        _play_pause_button.set_tooltip_text("Play / Pause");
        _play_pause_button.connect_signal_toggled(on_play_pause_button_toggled, this);
        _play_pause_button.set_child(&_play_pause_label);

        _play_pause_button.set_hexpand(false);
        _play_pause_button.set_vexpand(true);
        _play_pause_button.set_halign(GTK_ALIGN_START);

        _fps_spin_button.connect_signal_value_changed(on_fps_spin_button_value_changed, this);
        _scale_spin_button.connect_signal_value_changed(on_scale_spin_button_value_changed, this);
        _show_frame_i_check_button.connect_signal_toggled(on_show_frame_i_label_button_toggled, this);

        _fps_label.set_halign(GTK_ALIGN_START);
        _fps_separator.set_hexpand(true);
        _fps_separator.set_opacity(0);
        _fps_spin_button.set_halign(GTK_ALIGN_END);

        _fps_box.push_back(&_fps_label);
        _fps_box.push_back(&_fps_separator);
        _fps_box.push_back(&_fps_spin_button);
        _fps_box.set_tooltip_text("Number of Frames per Second");

        _scale_label.set_halign(GTK_ALIGN_START);
        _scale_separator.set_hexpand(true);
        _scale_separator.set_opacity(0);
        _scale_spin_button.set_halign(GTK_ALIGN_END);

        _scale_box.push_back(&_scale_label);
        _scale_box.push_back(&_scale_separator);
        _scale_box.push_back(&_scale_spin_button);
        _scale_box.set_tooltip_text("Scale: 1 Pixel in Image = N Pixels on Screen");

        _show_frame_i_label.set_halign(GTK_ALIGN_START);
        _show_frame_i_separator.set_hexpand(true);
        _show_frame_i_separator.set_opacity(0);
        _show_frame_i_check_button.set_halign(GTK_ALIGN_END);
        _show_frame_i_check_button.set_margin_start(2*state::margin_unit);

        _show_frame_i_box.push_back(&_show_frame_i_label);
        //_show_frame_i_box.push_back(&_show_frame_i_separator);
        _show_frame_i_box.push_back(&_show_frame_i_check_button);
        _show_frame_i_box.set_tooltip_text("Show Frame Index in Preview");

        _menu_popover_box.push_back(&_fps_box);
        _menu_popover_box.push_back(&_scale_box);
        _menu_popover_box.push_back(&_show_frame_i_box);
        _menu_popover.set_child(&_menu_popover_box);

        _menu_button.set_child(&_menu_button_label);
        _menu_button.set_popover(&_menu_popover);
        _menu_button.set_tooltip_text("Options");

        _toolbox_separator.set_hexpand(true);
        _toolbox_separator.set_vexpand(false);
        _toolbox_separator.set_opacity(0);

        _toolbox_box.push_back(&_play_pause_button);
        _toolbox_box.push_back(&_toolbox_separator);
        _toolbox_box.push_back(&_menu_button);
        _toolbox_box.set_hexpand(true);
        _toolbox_box.set_valign(GTK_ALIGN_END);
        _toolbox_box.set_homogeneous(true);

        _toolbox_revealer.set_child(&_toolbox_box);

        _motion_event_controller.connect_signal_motion_enter(on_motion_enter, this);
        _motion_event_controller.connect_signal_motion_leave(on_motion_leave, this);

        _layer_area.set_expand(false);
        _layer_area.set_align(GTK_ALIGN_CENTER);
        _layer_area.connect_signal_realize(on_layer_area_realize, this);
        _layer_area.connect_signal_resize(on_gl_area_resize, this);
        
        _transparency_tiling_area.set_expand(false);
        _transparency_tiling_area.set_align(GTK_ALIGN_CENTER);
        _transparency_tiling_area.connect_signal_realize(on_transparency_area_realize, this);
        _transparency_tiling_area.connect_signal_resize(on_gl_area_resize, this);

        _gl_area_overlay.set_child(&_transparency_tiling_area);
        _gl_area_overlay.add_overlay(&_layer_area);
        
        _gl_area_frame.set_ratio(state::layer_resolution.x / float(state::layer_resolution.y));
        _gl_area_frame.set_child(&_gl_area_overlay);

        _frame_label.set_halign(GTK_ALIGN_END);
        _frame_label.set_valign(GTK_ALIGN_START);
        _frame_label.set_expand(false);
        _frame_label.set_tooltip_text("Current Frame Index");

        _main.set_child(&_gl_area_frame);
        _main.add_overlay(&_toolbox_revealer);
        _main.add_overlay(&_frame_label);

        gtk_widget_add_tick_callback(_main.operator GtkWidget *(), (GtkTickCallback) G_CALLBACK(on_tick_callback), this, (GDestroyNotify) nullptr);
        _main.add_controller(&_motion_event_controller);
        _main.add_controller(&_key_event_controller);
        _key_event_controller.connect_signal_key_pressed(on_key_pressed, this);

        _fps_spin_button.set_value(_fps);
        _scale_spin_button.set_value(_scale);
        _play_pause_button.set_active(_is_unpaused);
        _show_frame_i_check_button.set_is_checked(_frame_i_visible);

        _toolbox_revealer.set_revealed(false);
    }

    Preview::operator Widget*()
    {
        return &_main;
    }

    void Preview::update()
    {
        for (auto* shape : _layer_shapes)
            delete shape;

        _layer_shapes.clear();

        _layer_area.make_current();
        _layer_area.clear_render_tasks();

        for (auto* layer : state::layers)
        {
            _layer_shapes.emplace_back(new Shape());
            _layer_shapes.back()->as_rectangle({0, 0}, {1, 1});
            _layer_shapes.back()->set_texture(layer->frames.at(_current_frame).texture);
            _layer_shapes.back()->set_visible(layer->is_visible);

            auto task = RenderTask(_layer_shapes.back(), nullptr, nullptr, layer->blend_mode);
            _layer_area.add_render_task(task);
        }

        _layer_area.queue_render();
    }
}