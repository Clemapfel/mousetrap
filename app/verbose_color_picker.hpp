// 
// Copyright 2022 Clemens Cords
// Created on 10/9/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <app/app_component.hpp>
#include <app/global_state.hpp>

#include <include/box.hpp>
#include <include/spin_button.hpp>
#include <include/shader.hpp>
#include <include/shape.hpp>
#include <include/gl_area.hpp>
#include <include/label.hpp>
#include <include/entry.hpp>
#include <include/button.hpp>
#include <include/get_resource_path.hpp>
#include <include/overlay.hpp>
#include <include/separator_line.hpp>
#include <include/event_controller.hpp>

namespace mousetrap
{
    class VerboseColorPicker : public AppComponent
    {
        public:
            VerboseColorPicker();
            operator Widget*() override;
            void update() override;

        private:
            void update(HSVA);

            HSVA* _current_color = new HSVA(state::primary_color);
            HSVA* _previous_color = new HSVA(state::secondary_color);
            
            static inline Shader* transparency_tiling_shader = nullptr;

            class CurrentColorRegion
            {
                public:
                    CurrentColorRegion(VerboseColorPicker*);
                    ~CurrentColorRegion();

                    operator Widget*();
                    void update(HSVA current, HSVA previous);

                private:
                    VerboseColorPicker* _owner;
                    
                    static inline const float _previous_to_color_current_width_ratio = 0.15;
                    
                    GLArea _gl_area;
                    static void on_gl_area_realize(Widget*, CurrentColorRegion*);
                    static void on_gl_area_resize(GLArea*, int, int, CurrentColorRegion*);
                    Vector2f _canvas_size;
                    
                    Shape* _current_color_shape;
                    Shape* _previous_color_shape;
                    Shape* _frame_shape;
                    Shape* _transparency_tiling_shape;
            };
            
            CurrentColorRegion _current_color_region;

            class HtmlCodeRegion
            {
                public:
                    HtmlCodeRegion(VerboseColorPicker*);
                    operator Widget*();
                    void update(HSVA);

                private:
                    VerboseColorPicker* _owner;

                    Label _label = Label("HTML Code:");
                    SeparatorLine _separator = SeparatorLine(GTK_ORIENTATION_HORIZONTAL);
                    Entry _entry;
                    Box _main = Box(GTK_ORIENTATION_HORIZONTAL);

                    static void on_entry_activate(Entry*, HtmlCodeRegion* instance);

                    static RGBA code_to_color(const std::string&);
                    static std::string color_to_code(RGBA);
                    static bool sanitize_code(std::string&);

            };

            HtmlCodeRegion _html_code_region;

            class Slider
            {
                public:
                    Slider(float* target, VerboseColorPicker* owner, bool use_hue_shader = false);
                    operator Widget*();

                    void update();

                    void set_left_color(RGBA);
                    void set_right_color(RGBA);

                private:
                    static inline float slider_width_to_cursor_width_ratio = 0.025;

                    float* _target;
                    VerboseColorPicker* _owner;
                    bool _use_shader;

                    static inline Shader* _bar_shader = nullptr;
                    Shape* _bar_shape;
                    Shape* _bar_frame_shape;

                    Shape* _cursor_shape;
                    Shape* _cursor_frame_shape;

                    Shape* _transparency_tiling_shape;
                    Vector2f _canvas_size;

                    GLArea _gl_area;
                    static void on_gl_area_realize(Widget*, Slider* instance);
                    static void on_gl_area_resize(GLArea*, int, int, Slider* instance);

                    bool _drag_active = false;

                    ClickEventController _click_event_controller;
                    static void on_slider_click_pressed(ClickEventController*, int, double, double, Slider*);
                    static void on_slider_click_released(ClickEventController*, int, double, double, Slider*);

                    MotionEventController _motion_event_controller;
                    static void on_slider_motion(MotionEventController*, double, double, Slider* instance);
            };

            std::deque<Slider> _sliders;

            Box _main = Box(GTK_ORIENTATION_VERTICAL);
    };
}

//

namespace mousetrap
{
    // CURRENT COLOR REGION

    VerboseColorPicker::CurrentColorRegion::CurrentColorRegion(VerboseColorPicker* owner)
        : _owner()
    {
        _gl_area.connect_signal_realize(on_gl_area_realize, this);
        _gl_area.connect_signal_resize(on_gl_area_resize, this);
        _gl_area.set_expand(true);
    }

    void VerboseColorPicker::CurrentColorRegion::on_gl_area_realize(Widget*, CurrentColorRegion* instance)
    {
        auto& area = instance->_gl_area;
        area.make_current();
        
        if (VerboseColorPicker::transparency_tiling_shader == nullptr)
        {
            VerboseColorPicker::transparency_tiling_shader = new Shader();
            VerboseColorPicker::transparency_tiling_shader->create_from_file(get_resource_path() + "shaders/transparency_tiling.frag", ShaderType::FRAGMENT);
        }

        instance->_transparency_tiling_shape = new Shape();
        instance->_transparency_tiling_shape->as_rectangle({0, 0}, {1, 1});

        instance->_previous_color_shape = new Shape();
        instance->_previous_color_shape->as_rectangle({0, 0}, {_previous_to_color_current_width_ratio, 1});
        instance->_previous_color_shape->set_color(state::primary_color);

        instance->_current_color_shape = new Shape();
        instance->_current_color_shape->as_rectangle({_previous_to_color_current_width_ratio, 0}, {1 - _previous_to_color_current_width_ratio, 1});
        instance->_current_color_shape->set_color(state::primary_color);

        {
            auto size = instance->_transparency_tiling_shape->get_size();
            size.y -= 0.0001;
            auto top_left = instance->_transparency_tiling_shape->get_top_left();
            top_left.x += 0.0001;

            std::vector<Vector2f> vertices = {
                    {top_left.x , top_left.y},
                    {top_left.x + size.x, top_left.y},
                    {top_left.x + size.x, top_left.y + size.y},
                    {top_left.x, top_left.y + size.y}
            };

            instance->_frame_shape = new Shape();
            instance->_frame_shape->as_wireframe(vertices);
            instance->_frame_shape->set_color(RGBA(0, 0, 0, 1));
        }

        instance->_transparency_tiling_shape = new Shape();
        instance->_transparency_tiling_shape->as_rectangle({0, 0}, {1, 1});

        auto transparency_render_task = RenderTask(instance->_transparency_tiling_shape, VerboseColorPicker::transparency_tiling_shader);
        transparency_render_task.register_vec2("_canvas_size", &instance->_canvas_size);

        area.add_render_task(transparency_render_task);
        area.add_render_task(instance->_current_color_shape);
        area.add_render_task(instance->_previous_color_shape);
        area.add_render_task(instance->_frame_shape);

        instance->update(state::primary_color, state::secondary_color);
        area.queue_render();
    }

    void VerboseColorPicker::CurrentColorRegion::on_gl_area_resize(GLArea*, int w, int h, CurrentColorRegion* instance)
    {
        instance->_canvas_size = {w, h};
        instance->_gl_area.queue_render();
    }

    void VerboseColorPicker::CurrentColorRegion::update(HSVA current, HSVA previous)
    {
        _current_color_shape->set_color(current.operator RGBA());
        _previous_color_shape->set_color(previous.operator RGBA());
        _gl_area.queue_render();
    }

    VerboseColorPicker::CurrentColorRegion::~CurrentColorRegion()
    {
        delete _current_color_shape;
        delete _previous_color_shape;
        delete _frame_shape;
        delete _transparency_tiling_shape;
    }

    VerboseColorPicker::CurrentColorRegion::operator Widget*()
    {
        return &_gl_area;
    }

    // HTML CODE REGION

    VerboseColorPicker::HtmlCodeRegion::HtmlCodeRegion(VerboseColorPicker* owner)
        : _owner(owner)
    {
        _separator.set_opacity(0);

        _main.push_back(&_label);
        _main.push_back(&_separator);
        _main.push_back(&_entry);

        _entry.connect_signal_activate(on_entry_activate, this);
        update(state::primary_color);
    }

    void VerboseColorPicker::HtmlCodeRegion::update(HSVA color)
    {
        _entry.set_text(color_to_code(color));
    }

    void VerboseColorPicker::HtmlCodeRegion::on_entry_activate(Entry* entry, HtmlCodeRegion* instance)
    {
        auto text = entry->get_text();
        bool is_valid = sanitize_code(text);

        if (is_valid)
        {
            entry->set_text(text);
            auto color = code_to_color(text);
            instance->_owner->update(color.operator HSVA());
        }
        else
            entry->set_text(color_to_code(*instance->_owner->_current_color));
    }

    std::string VerboseColorPicker::HtmlCodeRegion::color_to_code(RGBA in)
    {
        in.r = glm::clamp<float>(in.r, 0.f, 1.f);
        in.g = glm::clamp<float>(in.g, 0.f, 1.f);
        in.b = glm::clamp<float>(in.b, 0.f, 1.f);

        std::stringstream r;
        r << std::hex << int(std::round(in.r * 255));

        auto r_string = r.str();
        if (r_string.size() == 1)
        r_string = "0" + r_string;

        std::stringstream g;
        g << std::hex << int(std::round(in.g * 255));

        auto g_string = g.str();
        if (g_string.size() == 1)
        g_string = "0" + g_string;

        std::stringstream b;
        b << std::hex << int(std::round(in.b * 255));

        auto b_string = b.str();
        if (b_string.size() == 1)
        b_string = "0" + b_string;

        auto out = "#" + r_string + g_string + b_string;
        sanitize_code(out);
        return out;
    }

    RGBA VerboseColorPicker::HtmlCodeRegion::code_to_color(const std::string& text)
    {
        static auto hex_char_to_int = [](char c) -> uint8_t
        {
            if (c == '0')
                return 0;

            if (c == '1')
                return 1;

            if (c == '2')
                return 2;

            if (c == '3')
                return 3;

            if (c == '4')
                return 4;

            if (c == '5')
                return 5;

            if (c == '6')
                return 6;

            if (c == '7')
                return 7;

            if (c == '8')
                return 8;

            if (c == '9')
                return 9;

            if (c == 'A')
                return 10;

            if (c == 'B')
                return 11;

            if (c == 'C')
                return 12;

            if (c == 'D')
                return 13;

            if (c == 'E')
                return 14;

            if (c == 'F')
                return 15;

            return -1; // on error
        };

        static auto hex_component_to_int = [](int left, int right) -> uint8_t
        {
            return left * 16 + right;
        };

        std::vector<int> as_hex;
        as_hex.reserve(6);
        for (size_t i = 1; i < text.size(); ++i)
        {
            as_hex.push_back(hex_char_to_int(text.at(i)));
            if (as_hex.back() == -1)
                goto on_error;
        }

        return RGBA(
                hex_component_to_int(as_hex.at(0), as_hex.at(1)) / 255.f,
                hex_component_to_int(as_hex.at(2), as_hex.at(3)) / 255.f,
                hex_component_to_int(as_hex.at(4), as_hex.at(5)) / 255.f,
                1
        );

        on_error:
        std::cerr << "[WARNING] In VerboseColorPicker::HtmlCodeRegion::code_to_color: Unable to parse code \"" + text + "\"" << std::endl;
        return RGBA(0, 0, 0, 1);
    }

    bool VerboseColorPicker::HtmlCodeRegion::sanitize_code(std::string& in)
    {
        std::string text = "#";

        size_t start = 0;
        if (in.size() == 6)
            start = 0;
        else if (in.size() == 7)
            start = 1;
        else
            return false;

        for (size_t i = start; i < 6 + start; ++i)
        {
            auto c = in.at(i);
            if (c == 'a')
                c = 'A';
            else if (c == 'b')
                c = 'B';
            else if (c == 'c')
                c = 'C';
            else if (c == 'd')
                c = 'D';
            else if (c == 'e')
                c = 'E';
            else if (c == 'f')
                c = 'F';

            if (not ((c == '0') or (c == '1') or (c == '2') or (c == '3') or (c == '4') or (c == '5') or (c == '6') or (c == '7') or (c == '8') or (c == '9') or (c == 'A') or (c == 'B') or (c == 'C') or (c == 'D') or (c == 'E') or (c == 'F')))
                return false;

            text.push_back(c);
        }

        std::cout << text << std::endl;

        in = text;
        return true;
    }

    VerboseColorPicker::HtmlCodeRegion::operator Widget*()
    {
        return &_main;
    }

    // SLIDER

    VerboseColorPicker::Slider::Slider(float* target, VerboseColorPicker* owner, bool use_hue_shader)
        : _target(target), _owner(owner), _use_shader(use_hue_shader)
    {
        _gl_area.connect_signal_realize(on_gl_area_realize, this);
        _gl_area.connect_signal_resize(on_gl_area_resize, this);

        _click_event_controller.connect_signal_click_pressed(on_slider_click_pressed, this);
        _click_event_controller.connect_signal_click_released(on_slider_click_released, this);
        _gl_area.add_controller(&_click_event_controller);

        _motion_event_controller.connect_signal_motion(on_slider_motion, this);
        _gl_area.add_controller(&_motion_event_controller);

        _gl_area.set_expand(true);
    }

    VerboseColorPicker::Slider::operator Widget*()
    {
        return &_gl_area;
    }

    void VerboseColorPicker::Slider::on_gl_area_realize(Widget*, Slider* instance)
    {
        auto& area = instance->_gl_area;
        area.make_current();

        instance->_cursor_shape = new Shape();
        instance->_cursor_shape->as_rectangle({0.5, 0}, {instance->slider_width_to_cursor_width_ratio, 1});

        {
            auto size = instance->_cursor_shape->get_size();
            size.y -= 0.001;
            auto top_left = instance->_cursor_shape->get_top_left();

            std::vector<Vector2f> vertices = {
                    {top_left.x , top_left.y},
                    {top_left.x + size.x, top_left.y},
                    {top_left.x + size.x, top_left.y + size.y},
                    {top_left.x, top_left.y + size.y}
            };

            instance->_cursor_frame_shape = new Shape();
            instance->_cursor_frame_shape->as_wireframe(vertices);
            instance->_cursor_frame_shape->set_color(RGBA(0, 0, 0, 1));
        }

        static float bar_margin = 0.15;

        instance->_bar_shape = new Shape();
        instance->_bar_shape->as_rectangle({0, bar_margin}, {1, 1 - 2 * bar_margin});

        {
            auto size = instance->_bar_shape->get_size();
            auto top_left = instance->_bar_shape->get_top_left();
            size.x -= 0.0002;
            top_left.x += 0.0001;

            std::vector<Vector2f> vertices = {
                    {top_left.x , top_left.y},
                    {top_left.x + size.x, top_left.y},
                    {top_left.x + size.x, top_left.y + size.y},
                    {top_left.x, top_left.y + size.y}
            };

            instance->_bar_frame_shape = new Shape();
            instance->_bar_frame_shape->as_wireframe(vertices);
            instance->_bar_frame_shape->set_color(RGBA(0, 0, 0, 1));
        }

        if (VerboseColorPicker::transparency_tiling_shader == nullptr)
        {
            VerboseColorPicker::transparency_tiling_shader = new Shader();
            VerboseColorPicker::transparency_tiling_shader->create_from_file(get_resource_path() + "shaders/transparency_tiling.frag", ShaderType::FRAGMENT);
        }

        instance->_transparency_tiling_shape = new Shape();
        instance->_transparency_tiling_shape->as_rectangle(instance->_bar_shape->get_top_left(), instance->_bar_shape->get_size());
        auto transparency_render_task = RenderTask(instance->_transparency_tiling_shape, transparency_tiling_shader);
        transparency_render_task.register_vec2("_canvas_size", &instance->_canvas_size);

        instance->_gl_area.add_render_task(transparency_render_task);

        if (_bar_shader == nullptr)
        {
            _bar_shader = new Shader();
            _bar_shader->create_from_file(get_resource_path() + "shaders/verbose_color_picker_hue_gradient.frag", ShaderType::FRAGMENT);
        }

        if (instance->_use_shader)
        {
            auto bar_render_task = RenderTask(instance->_bar_shape, instance->_bar_shader);
            bar_render_task.register_color("_current_color_hsva", instance->_owner->_current_color);
            instance->_gl_area.add_render_task(bar_render_task);
        }
        else
            instance->_gl_area.add_render_task(instance->_bar_shape);

        instance->_gl_area.add_render_task(instance->_bar_frame_shape);
        instance->_gl_area.add_render_task(instance->_cursor_shape);
        instance->_gl_area.add_render_task(instance->_cursor_frame_shape);

        instance->update();
        area.queue_render();
    };

    void VerboseColorPicker::Slider::on_gl_area_resize(GLArea*, int w, int h, Slider* instance)
    {
        instance->_canvas_size = {w, h};
        instance->_gl_area.queue_render();
    }

    void VerboseColorPicker::Slider::on_slider_click_pressed(ClickEventController*, int, double x, double y, Slider* instance)
    {
        auto pos = Vector2f{x, y};
        pos.x /= instance->_canvas_size.x;
        pos.y /= instance->_canvas_size.y;

        instance->_drag_active = true;
        instance->_gl_area.set_cursor(GtkCursorType::GRABBING);
        auto value = glm::clamp<float>(pos.x, 0, 1);

        *instance->_target = value;
        instance->update();
    }

    void VerboseColorPicker::Slider::on_slider_click_released(ClickEventController*, int, double x, double y, Slider* instance)
    {
        auto pos = Vector2f{x, y};
        pos.x /= instance->_canvas_size.x;
        pos.y /= instance->_canvas_size.y;

        if (instance->_drag_active)
        {
            auto value = glm::clamp<float>(pos.x, 0, 1);
            auto color = state::primary_color;

            *instance->_target = value;
            instance->update();

            instance->_drag_active = false;
            instance->_gl_area.set_cursor(GtkCursorType::GRAB);
        }
    }

    void VerboseColorPicker::Slider::on_slider_motion(MotionEventController*, double x, double y, Slider* instance)
    {
        if (instance->_drag_active)
        {
            auto pos = Vector2f(x, y);
            pos.x /= instance->_canvas_size.x;
            pos.y /= instance->_canvas_size.y;
            auto value = glm::clamp<float>(pos.x, 0, 1);

            *instance->_target = value;
            instance->update();
        }
    }

    void VerboseColorPicker::Slider::update()
    {
        if (not _gl_area.get_is_realized())
            return;

        auto value = *_target;

        if (value < 0.001)
            value = 0;

        if (value > 1 - 0.001)
            value = 1;

        Vector2f centroid = {value, 0.5};

        if (centroid.x < _cursor_shape->get_size().x * 0.5)
            centroid.x = _cursor_shape->get_size().x * 0.5;

        if (centroid.x > 1 - _cursor_shape->get_size().x * 0.5)
            centroid.x = 1 - _cursor_shape->get_size().x * 0.5;

        _cursor_shape->set_centroid(centroid);
        _cursor_frame_shape->set_centroid(centroid);

        _gl_area.queue_render();
    }

    // VERBOSE COLOR PICKER

    VerboseColorPicker::VerboseColorPicker()
        : _current_color_region(this), _html_code_region(this)
    {
        //_main.push_back(_current_color_region);
        //_main.push_back(_html_code_region);

        _sliders.emplace_back(&_current_color->v, this, true);

        for (auto& slider : _sliders)
            _main.push_back(slider);
    }

    VerboseColorPicker::operator Widget*()
    {
        return &_main;
    }

    void VerboseColorPicker::update(HSVA)
    {}

    void VerboseColorPicker::update()
    {
        update(state::primary_color);
    }
}