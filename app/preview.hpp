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

#include <app/global_state.hpp>

namespace mousetrap
{
    class Preview : public AppComponent
    {
        public:
            Preview();
            ~Preview();

            operator Widget*() override;
            void update() override {};

            // zoom level
            // pause / unpause
            // fps slider

        private:
            GLArea _gl_area;
            static void on_gl_area_realize(Widget*, Preview*);
            static void on_gl_area_resize(GLArea*, int, int, Preview*);

            Shape* _transparency_tiling_shape;
            Shader _transparency_tiling_shader;

            Vector2f _canvas_size;
            AspectFrame _gl_area_frame = AspectFrame(1);

            SpinButton _fps_spin_button = SpinButton(1, 99, 1);
            static void on_fps_spin_button_value_changed(SpinButton*, Preview*);

            ToggleButton _play_pause_button;
            Label _play_pause_label = Label("<tt>&#9654;</tt>");
            static void on_play_pause_button_toggled(ToggleButton*, Preview*);

            Box _toolbox_box = Box(GTK_ORIENTATION_HORIZONTAL);
            SeparatorLine _toolbox_separator;
            Revealer _toolbox_revealer;

            MotionEventController _motion_event_controller;
            static void on_motion_enter(MotionEventController*, double x, double y, Preview* instance);
            static void on_motion_leave(MotionEventController*, Preview* instance);

            Overlay _main;

    };
}

namespace mousetrap
{
    Preview::~Preview()
    {
        delete _transparency_tiling_shape;
    }

    void Preview::on_gl_area_realize(Widget* widget, Preview* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        instance->_transparency_tiling_shape = new Shape();
        instance->_transparency_tiling_shape->as_rectangle({0, 0}, {1, 1});

        instance->_transparency_tiling_shader = Shader();
        instance->_transparency_tiling_shader.create_from_file(get_resource_path() + "shaders/transparency_tiling.frag", ShaderType::FRAGMENT);

        auto task = RenderTask(instance->_transparency_tiling_shape, &instance->_transparency_tiling_shader);
        task.register_vec2("_canvas_size", &instance->_canvas_size);

        area->add_render_task(task);
        area->queue_render();
    }

    void Preview::on_gl_area_resize(GLArea* area, int w, int h, Preview* instance)
    {
        instance->_canvas_size = {w, h};
        area->queue_render();
    }

    void Preview::on_play_pause_button_toggled(ToggleButton* button, Preview* instance)
    {
    }

    void Preview::on_fps_spin_button_value_changed(SpinButton* scale, Preview* instance)
    {
        float value = scale->get_value();
    }

    void Preview::on_motion_enter(MotionEventController*, double x, double y, Preview* instance)
    {
        instance->_toolbox_revealer.set_revealed(true);
    }

    void Preview::on_motion_leave(MotionEventController*, Preview* instance)
    {
        instance->_toolbox_revealer.set_revealed(false);
    }

    Preview::Preview()
    {
        _fps_spin_button.set_tooltip_text("FPS (Number of Frames per Second)");
        _fps_spin_button.connect_signal_value_changed(on_fps_spin_button_value_changed, this);
        _fps_spin_button.set_expand(false);
        _fps_spin_button.set_halign(GTK_ALIGN_END);

        _play_pause_button.set_tooltip_text("Play / Pause");
        _play_pause_button.connect_signal_toggled(on_play_pause_button_toggled, this);
        _play_pause_button.set_child(&_play_pause_label);

        _play_pause_button.set_hexpand(false);
        _play_pause_button.set_vexpand(true);
        _play_pause_button.set_halign(GTK_ALIGN_START);

        _toolbox_separator.set_hexpand(true);
        _toolbox_separator.set_vexpand(false);
        _toolbox_separator.set_opacity(0);

        _toolbox_box.push_back(&_play_pause_button);
        _toolbox_box.push_back(&_toolbox_separator);
        _toolbox_box.push_back(&_fps_spin_button);
        _toolbox_box.set_hexpand(true);
        _toolbox_box.set_valign(GTK_ALIGN_END);

        _toolbox_revealer.set_child(&_toolbox_box);

        _motion_event_controller.connect_signal_motion_enter(on_motion_enter, this);
        _motion_event_controller.connect_signal_motion_leave(on_motion_leave, this);

        std::cout << state::layer_resolution.x << " " << state::layer_resolution.y << std::endl;

        _gl_area.set_size_request(state::layer_resolution * Vector2ui(2));
        _gl_area.connect_signal_realize(on_gl_area_realize, this);
        _gl_area.connect_signal_resize(on_gl_area_resize, this);

        _gl_area_frame.set_ratio(state::layer_resolution.x / float(state::layer_resolution.y));
        _gl_area_frame.set_child(&_gl_area);

        _main.set_child(&_gl_area_frame);
        _main.add_overlay(&_toolbox_revealer);

        _main.add_controller(&_motion_event_controller);
        _main.set_size_request(state::layer_resolution * Vector2ui(2));
    }

    Preview::operator Widget*()
    {
        return &_main;
    }
}