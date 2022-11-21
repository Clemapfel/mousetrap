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

            void set_color_component_to(char c, float value);
            static inline Shader* transparency_tiling_shader = nullptr;

            class HtmlCodeRegion
            {
                public:
                    HtmlCodeRegion(VerboseColorPicker*);
                    operator Widget*();
                    void update(HSVA);

                private:
                    VerboseColorPicker* _owner;

                    Label _label = Label("HTML Code: ");
                    SeparatorLine _separator = SeparatorLine(GTK_ORIENTATION_HORIZONTAL);
                    Entry _entry;
                    Box _main = Box(GTK_ORIENTATION_HORIZONTAL);

                    static void on_entry_activate(Entry*, HtmlCodeRegion* instance);

                    static RGBA code_to_color(const std::string&);
                    static std::string color_to_code(RGBA);
                    static bool sanitize_code(std::string&);
            };

            HtmlCodeRegion _html_code_region;

            class SliderRegion
            {
                public:
                    SliderRegion(char target_id, VerboseColorPicker* owner, bool use_hue_shader = false);
                    operator Widget*();

                    void update();
                    void set_value(float);

                    void set_left_color(RGBA);
                    void set_right_color(RGBA);

                private:
                    static inline const float slider_width = state::margin_unit * 0.5;
                    static inline const float bar_margin = 0.15;

                    char _target_id;
                    float _value;
                    VerboseColorPicker* _owner;
                    bool _use_shader;

                    static inline Shader* _bar_shader = nullptr;
                    Shape* _bar_shape;
                    Shape* _bar_frame_shape;

                    Shape* _cursor_shape;
                    Shape* _cursor_frame_shape;

                    Shape* _transparency_tiling_shape;
                    Vector2f _canvas_size = {1, 1};

                    GLArea _gl_area;
                    static void on_gl_area_realize(Widget*, SliderRegion* instance);
                    static void on_gl_area_resize(GLArea*, int, int, SliderRegion* instance);

                    bool _drag_active = false;

                    ClickEventController _click_event_controller;
                    static void on_slider_click_pressed(ClickEventController*, int, double, double, SliderRegion*);
                    static void on_slider_click_released(ClickEventController*, int, double, double, SliderRegion*);

                    MotionEventController _motion_event_controller;
                    static void on_slider_motion(MotionEventController*, double, double, SliderRegion* instance);

                    SpinButton _spin_button = SpinButton(0, 1, 0.001);
                    static void on_spin_button_value_changed(SpinButton*, SliderRegion* instance);

                    Label _label;
                    Box _main = Box(GTK_ORIENTATION_HORIZONTAL);
            };

            std::unordered_map<char, SliderRegion*> _sliders;

            Label _opacity_label = Label("Opacity");
            Label _hsv_label = Label("HSV");
            Label _rgb_label = Label("RGB");

            Box _main = Box(GTK_ORIENTATION_VERTICAL);

            Tooltip _tooltip;
    };
}

//

namespace mousetrap
{
    // HTML CODE REGION

    VerboseColorPicker::HtmlCodeRegion::HtmlCodeRegion(VerboseColorPicker* owner)
        : _owner(owner)
    {
        _separator.set_opacity(0);

        _main.push_back(&_label);
        _main.push_back(&_separator);
        _main.push_back(&_entry);

        _label.set_hexpand(false);
        _label.set_halign(GTK_ALIGN_START);
        _separator.set_hexpand(true);
        _entry.set_hexpand(false);
        _entry.set_halign(GTK_ALIGN_END);

        _label.set_vexpand(true);
        _separator.set_vexpand(true);
        _entry.set_vexpand(true);

        _entry.connect_signal_activate(on_entry_activate, this);
        _entry.set_n_chars(7 + 1);
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

        in = text;
        return true;
    }

    VerboseColorPicker::HtmlCodeRegion::operator Widget*()
    {
        return &_main;
    }

    // SLIDER

    VerboseColorPicker::SliderRegion::SliderRegion(char target_id, VerboseColorPicker* owner, bool use_hue_shader)
        : _target_id(target_id), _owner(owner), _use_shader(use_hue_shader), _label("<tt>" + std::string({(char) std::toupper(target_id)}) + ":</tt>")
    {
        _gl_area.connect_signal_realize(on_gl_area_realize, this);
        _gl_area.connect_signal_resize(on_gl_area_resize, this);

        _click_event_controller.connect_signal_click_pressed(on_slider_click_pressed, this);
        _click_event_controller.connect_signal_click_released(on_slider_click_released, this);
        _gl_area.add_controller(&_click_event_controller);

        _motion_event_controller.connect_signal_motion(on_slider_motion, this);
        _gl_area.add_controller(&_motion_event_controller);
        _gl_area.set_expand(true);
        _gl_area.set_cursor(GtkCursorType::GRAB);

        _spin_button.set_margin_start(state::margin_unit);
        _spin_button.connect_signal_value_changed(on_spin_button_value_changed, this);
        _spin_button.set_wrap(_target_id == 'h' or _target_id == 'H');

        _main.push_back(&_label);
        _main.push_back(&_gl_area);
        _main.push_back(&_spin_button);

        _spin_button.set_hexpand(false);

        update();
    }

    VerboseColorPicker::SliderRegion::operator Widget*()
    {
        return &_main;
    }

    void VerboseColorPicker::SliderRegion::on_gl_area_realize(Widget*, SliderRegion* instance)
    {
        auto& area = instance->_gl_area;
        area.make_current();

        instance->_cursor_shape = new Shape();
        instance->_cursor_frame_shape = new Shape();
        // shapes set during resize

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

        instance->_owner->update(*instance->_owner->_current_color);
        area.queue_render();
    };

    void VerboseColorPicker::SliderRegion::on_gl_area_resize(GLArea*, int w, int h, SliderRegion* instance)
    {
        instance->_canvas_size = {w, h};
        instance->set_value(instance->_value);

        Vector2f cursor_pos = {instance->_value, 0};
        Vector2f cursor_size = {instance->slider_width / instance->_canvas_size.x, 1};

        if (cursor_pos.x < cursor_size.x * 0.5)
            cursor_pos.x = cursor_size.x * 0.5;

        if (cursor_pos.x > 1 - cursor_size.x * 0.5)
            cursor_pos.x = 1 - cursor_size.x * 0.5;

        cursor_pos.x -= std::fmod(cursor_pos.x, 1.f / instance->_canvas_size.x); // align to pixelgrid

        instance->_cursor_shape->as_rectangle(cursor_pos, cursor_size);
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

            instance->_cursor_frame_shape->as_wireframe(vertices);
            instance->_cursor_frame_shape->set_color(RGBA(0, 0, 0, 1));
        }

        instance->_gl_area.queue_render();
    }

    void VerboseColorPicker::SliderRegion::on_slider_click_pressed(ClickEventController*, int, double x, double y, SliderRegion* instance)
    {
        auto pos = Vector2f{x, y};
        pos.x /= instance->_canvas_size.x;
        pos.y /= instance->_canvas_size.y;

        instance->_drag_active = true;
        instance->_gl_area.set_cursor(GtkCursorType::GRABBING);
        auto value = glm::clamp<float>(pos.x, 0, 1);

        instance->_value = value;
        instance->_owner->set_color_component_to(instance->_target_id, value);
        instance->_owner->update(*instance->_owner->_current_color);
    }

    void VerboseColorPicker::SliderRegion::on_slider_click_released(ClickEventController*, int, double x, double y, SliderRegion* instance)
    {
        auto pos = Vector2f{x, y};
        pos.x /= instance->_canvas_size.x;
        pos.y /= instance->_canvas_size.y;

        if (instance->_drag_active)
        {
            auto value = glm::clamp<float>(pos.x, 0, 1);
            auto color = state::primary_color;

            instance->_value = value;
            instance->_owner->set_color_component_to(instance->_target_id, value);
            instance->_owner->update(*instance->_owner->_current_color);

            instance->_drag_active = false;
            instance->_gl_area.set_cursor(GtkCursorType::GRAB);
        }

        *instance->_owner->_previous_color = *instance->_owner->_current_color;

        state::primary_color = *instance->_owner->_current_color;
        state::update_color_picker();
        state::update_color_swapper();
        state::update_palette_view();

        state::preview_color_current = state::primary_color;
        state::preview_color_previous = state::primary_color;
        state::update_color_preview();
    }

    void VerboseColorPicker::SliderRegion::on_slider_motion(MotionEventController*, double x, double y, SliderRegion* instance)
    {
        if (instance->_drag_active)
        {
            auto pos = Vector2f(x, y);
            pos.x /= instance->_canvas_size.x;
            pos.y /= instance->_canvas_size.y;
            auto value = glm::clamp<float>(pos.x, 0, 1);

            instance->_value = value;
            instance->_owner->set_color_component_to(instance->_target_id, value);
            instance->_owner->update(*instance->_owner->_current_color);
        }
    }

    void VerboseColorPicker::SliderRegion::on_spin_button_value_changed(SpinButton* button, SliderRegion* instance)
    {
        float value = button->get_value();
        instance->_value = value;
        instance->_owner->set_color_component_to(instance->_target_id, value);
        instance->_owner->update(*instance->_owner->_current_color);
    }

    void VerboseColorPicker::SliderRegion::set_value(float value)
    {
        _value = value;
        update();
    }

    void VerboseColorPicker::SliderRegion::update()
    {
        auto value = _value;

        if (value < 0.001)
            value = 0;

        if (value > 1 - 0.001)
            value = 1;

        if (_gl_area.get_is_realized())
        {
            Vector2f centroid = {value, 0.5};

            if (centroid.x < _cursor_shape->get_size().x * 0.5)
                centroid.x = _cursor_shape->get_size().x * 0.5;

            if (centroid.x > 1 - _cursor_shape->get_size().x * 0.5)
                centroid.x = 1 - _cursor_shape->get_size().x * 0.5;

            centroid.x -= std::fmod(centroid.x, 1.f / _canvas_size.x); // align to pixelgrid

            _cursor_shape->set_centroid(centroid);
            _cursor_frame_shape->set_centroid(centroid);

            _gl_area.queue_render();
        }

        _spin_button.set_signal_value_changed_blocked(true);
        _spin_button.set_value(value);
        _spin_button.set_signal_value_changed_blocked(false);
    }

    void VerboseColorPicker::SliderRegion::set_left_color(RGBA color)
    {
        if (not _gl_area.get_is_realized())
            return;

        _bar_shape->set_vertex_color(0, color);
        _bar_shape->set_vertex_color(3, color);
        _gl_area.queue_render();
    }

    void VerboseColorPicker::SliderRegion::set_right_color(RGBA color)
    {
        if (not _gl_area.get_is_realized())
            return;

        _bar_shape->set_vertex_color(1, color);
        _bar_shape->set_vertex_color(2, color);
        _gl_area.queue_render();
    }

    // VERBOSE COLOR PICKER

    void VerboseColorPicker::set_color_component_to(char c, float value)
    {
        auto color = *_current_color;
        float h_before = color.h;

        if (c == 'A' or c == 'a')
        color.a = value;
        else if (c == 'H' or c == 'h')
        color.h = value;
        else if (c == 'S' or c == 's')
        color.s = value;
        else if (c == 'V' or c == 'v')
        color.v = value;
        else
        {
            auto as_rgba = color.operator RGBA();
            if (c == 'R' or c == 'r')
            as_rgba.r = value;
            else if (c == 'G' or c == 'g')
            as_rgba.g = value;
            else if (c == 'B' or c == 'b')
            as_rgba.b = value;

            color = as_rgba.operator HSVA();
            if (color.s == 0)
            color.h = h_before;
        }

        *_current_color = color;
        state::primary_color = color;
        state::update_color_picker();
        state::update_color_swapper();
        state::update_palette_view();

        state::preview_color_current = color;
        state::update_color_preview();
    };

    VerboseColorPicker::VerboseColorPicker()
        : _html_code_region(this)
    {
        auto add_slider = [&](char id)
        {
            return _sliders.insert({id, new SliderRegion(id, this, id == 'h' or id == 'H')});
        };

        for (char c : {'a', 'h', 's', 'v', 'r', 'g', 'b'})
        {
            auto* slider = add_slider(c).first->second;

            if (c == 'a')
            {
                slider->set_value(_current_color->a);
            }
            else if (c == 'h')
            {
                slider->set_value(_current_color->h);
            }
            else if (c == 's')
            {
                slider->set_value(_current_color->s);
            }
            else if (c == 'v')
            {
                slider->set_value(_current_color->v);
            }
            else if (c == 'r')
            {
                slider->set_value(_current_color->operator RGBA().r);
            }
            else if (c == 'g')
            {
                slider->set_value(_current_color->operator RGBA().g);
            }
            else if (c == 'b')
            {
                slider->set_value(_current_color->operator RGBA().b);
            }
        }

        for (auto* label : {&_opacity_label, &_hsv_label, &_rgb_label})
        {
            label->set_halign(GTK_ALIGN_START);
        }

        for (auto& pair : _sliders)
            pair.second->operator Widget *()->set_margin_start(2 * state::margin_unit);

        _html_code_region.operator Widget *()->set_margin_top(state::margin_unit);

        //_main.push_back(&_opacity_label);
        //_main.push_back(_sliders.at('a')->operator Widget*());

        _main.push_back(&_hsv_label);
        _main.push_back(_sliders.at('h')->operator Widget*());
        _main.push_back(_sliders.at('s')->operator Widget*());
        _main.push_back(_sliders.at('v')->operator Widget*());

        _main.push_back(&_rgb_label);
        _main.push_back(_sliders.at('r')->operator Widget*());
        _main.push_back(_sliders.at('g')->operator Widget*());
        _main.push_back(_sliders.at('b')->operator Widget*());
        _main.push_back(_html_code_region);

        _sliders.at('a')->operator Widget*()->set_tooltip_text(state::tooltips_file->get_value("verbose_color_picker", "opacity"));
        _sliders.at('h')->operator Widget*()->set_tooltip_text(state::tooltips_file->get_value("verbose_color_picker", "hue"));
        _sliders.at('s')->operator Widget*()->set_tooltip_text(state::tooltips_file->get_value("verbose_color_picker", "saturation"));
        _sliders.at('v')->operator Widget*()->set_tooltip_text(state::tooltips_file->get_value("verbose_color_picker", "value"));
        _sliders.at('r')->operator Widget*()->set_tooltip_text(state::tooltips_file->get_value("verbose_color_picker", "red"));
        _sliders.at('g')->operator Widget*()->set_tooltip_text(state::tooltips_file->get_value("verbose_color_picker", "green"));
        _sliders.at('b')->operator Widget*()->set_tooltip_text(state::tooltips_file->get_value("verbose_color_picker", "blue"));
        _html_code_region.operator Widget*()->set_tooltip_text(state::tooltips_file->get_value("verbose_color_picker", "html_code"));

        _tooltip.create_from("verbose_color_picker", state::tooltips_file, state::keybindings_file);
        operator Widget*()->set_tooltip_widget(_tooltip);

        _main.set_homogeneous(false);
        for (Label* l : {&_opacity_label, &_hsv_label, &_rgb_label})
            l->set_vexpand(false);

        _html_code_region.operator Widget*()->set_vexpand(false);
    }

    VerboseColorPicker::operator Widget*()
    {
        return &_main;
    }

    void VerboseColorPicker::update(HSVA color_hsva)
    {
        auto color_rgba = color_hsva.operator RGBA();

        auto it = _sliders.find('h');
        if (it != _sliders.end())
        {
            auto& slider = *it->second;
            slider.set_value(color_hsva.h);
        }

        it = _sliders.find('s');
        if (it != _sliders.end())
        {
            auto& slider = *it->second;
            slider.set_value(color_hsva.s);
            slider.set_left_color(HSVA(color_hsva.h, 0, color_hsva.v, color_hsva.a));
            slider.set_right_color(HSVA(color_hsva.h, 1, color_hsva.v, color_hsva.a));
        }

        it = _sliders.find('v');
        if (it != _sliders.end())
        {
            auto& slider = *it->second;
            slider.set_value(color_hsva.v);
            slider.set_left_color(HSVA(color_hsva.h, color_hsva.s, 0, color_hsva.a));
            slider.set_right_color(HSVA(color_hsva.h, color_hsva.s, 1, color_hsva.a));
        }

        it = _sliders.find('a');
        if (it != _sliders.end())
        {
            auto& slider = *it->second;
            slider.set_value(color_hsva.a);
            slider.set_left_color(HSVA(color_hsva.h, color_hsva.s, color_hsva.v, 0));
            slider.set_right_color(HSVA(color_hsva.h, color_hsva.s, color_hsva.v, 1));
        }

        it = _sliders.find('r');
        if (it != _sliders.end())
        {
            auto& slider = *it->second;
            slider.set_value(color_rgba.r);
            slider.set_left_color(RGBA(0, color_rgba.g, color_rgba.b, color_rgba.a));
            slider.set_right_color(RGBA(1, color_rgba.g, color_rgba.b, color_rgba.a));
        }

        it = _sliders.find('g');
        if (it != _sliders.end())
        {
            auto& slider = *it->second;
            slider.set_value(color_rgba.g);
            slider.set_left_color(RGBA(color_rgba.r, 0, color_rgba.b, color_rgba.a));
            slider.set_right_color(RGBA(color_rgba.r, 1, color_rgba.b, color_rgba.a));
        }

        it = _sliders.find('b');
        if (it != _sliders.end())
        {
            auto& slider = *it->second;
            slider.set_value(color_rgba.b);
            slider.set_left_color(RGBA(color_rgba.r, color_rgba.g, 0, color_rgba.a));
            slider.set_right_color(RGBA(color_rgba.r, color_rgba.g, 1, color_rgba.a));
        }

        _html_code_region.update(*_current_color);
    }

    void VerboseColorPicker::update()
    {
        *_current_color = state::primary_color;
        *_previous_color = state::primary_color;
        update(state::primary_color);
    }
}