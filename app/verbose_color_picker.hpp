// 
// Copyright 2022 Clemens Cords
// Created on 8/17/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

#include <set>

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

#include <app/global_state.hpp>

namespace mousetrap
{
    class VerboseColorPicker : public Widget
    {
        public:
            VerboseColorPicker();
            ~VerboseColorPicker();

            operator GtkWidget*() override;

            void update() override;

        private:
            void update(HSVA color);

            Box* _all_regions_box;
            Overlay* _main;

            // slider regions

            struct SliderRegion
            {
                Label* _label;
                SeparatorLine* _separator;
                Box* _label_separator_box;
                Box* _main;

                ~SliderRegion() {
                    delete _label;
                    delete _label_separator_box;
                    delete _main;
                }
            };

            SliderRegion* _opacity_region;
            SliderRegion* _hsv_region;
            SliderRegion* _rgb_region;

            // slider

            struct Slider
            {
                Shader* _bar_shader;
                Shape* _cursor;
                Shape* _cursor_frame;

                Shape* _bar;
                Shape* _bar_frame;

                Shape* _transparency_background;

                GLArea* _canvas;
                SpinButton* _spin_button;

                Box* _main;

                Vector2f* _canvas_size;
                static inline Shader* _transparency_shader = nullptr;

                static inline float _width_to_cursor_width_ratio = 0.025;
                bool _drag_active = false;

                float value;
                void set_value(float x);

                ~Slider() {
                    delete _bar_shader;
                    delete _cursor;
                    delete _cursor_frame;
                    delete _bar;
                    delete _bar_frame;
                    delete _transparency_background;
                    delete _canvas_size;
                }

                static inline HSVA _hue_gradient_shader_current_color = state::primary_color;
            };

            std::map<char, Slider*> _sliders
            {
                {'A', nullptr},
                {'H', nullptr},
                {'S', nullptr},
                {'V', nullptr},
                {'R', nullptr},
                {'G', nullptr},
                {'B', nullptr}
            };

            static HSVA get_color_with_component_set_to(HSVA color, char component, float value);

            using SliderTuple_t = std::tuple<char, VerboseColorPicker*>;
            static void on_slider_realize(GtkGLArea* area, SliderTuple_t*);
            static void on_slider_resize(GtkGLArea* area, int w, int h, SliderTuple_t*);
            static void on_slider_spin_button_value_changed(GtkSpinButton*, SliderTuple_t*);

            //static void on_slider_motion_notify_event(GtkWidget *widget, GdkEventMotion *event, SliderTuple_t*);
            //static void on_slider_button_press_event(GtkWidget* widget, GdkEventButton* event, SliderTuple_t*);
            //static void on_slider_button_release_event(GtkWidget* widget, GdkEventButton* event, SliderTuple_t*);

            // html region

            struct HtmlRegion
            {
                Box * _label_hbox;
                Label* _label;
                SeparatorLine* _label_separator;

                Box* _entry_hbox;
                Entry* _entry;
                SeparatorLine* _entry_to_button_separator;
                Button* _button; // TODO

                Box* _main;
            };

            HtmlRegion* _html_region;
            static void on_entry_activate(GtkEntry*, VerboseColorPicker* instance);
            static void on_entry_paste(GtkEntry*, VerboseColorPicker* instance);

            std::string sanitize_html_code(const std::string&);
            std::string color_to_html_code(RGBA);
            RGBA html_code_to_color(const std::string&);
            bool is_html_code_valid(const std::string&);

            // current color

            struct CurrentColorRegion
            {
                GLArea* _canvas;
                Shape* _current_color_shape;
                Shape* _last_color_shape;       // updates on button release
                Shape* _frame;
                Shape* _transparency_background;

                Vector2f* _canvas_size;
                Box* _main;

                static inline Shader* _transparency_shader = nullptr;
                static inline float _width_to_last_color_width_ration = 0.15;

                ~CurrentColorRegion()
                {
                    delete _current_color_shape;
                    delete _last_color_shape;
                    delete _frame;
                    delete _transparency_background;
                    delete _canvas_size;
                }
            };

            CurrentColorRegion* _current_color_region;

            static void on_current_color_region_realize(GtkGLArea* area, VerboseColorPicker* instance);
            static void on_current_color_region_resize(GtkGLArea* area, int w, int h, VerboseColorPicker* instance);
    };
}

// ###

namespace mousetrap
{
    VerboseColorPicker::VerboseColorPicker()
    {
        _opacity_region = new SliderRegion {
            new Label("Opacity"),
            new SeparatorLine(),
            new Box(GTK_ORIENTATION_HORIZONTAL),
            new Box(GTK_ORIENTATION_VERTICAL)
        };

        _opacity_region->_label_separator_box->push_back(_opacity_region->_label);
        _opacity_region->_label_separator_box->push_back(_opacity_region->_separator);
        _opacity_region->_main->push_back(_opacity_region->_label_separator_box);

        _hsv_region = new SliderRegion {
                new Label("HSV"),
                new SeparatorLine(),
                new Box(GTK_ORIENTATION_HORIZONTAL),
                new Box(GTK_ORIENTATION_VERTICAL)
        };

        _hsv_region->_label_separator_box->push_back(_hsv_region->_label);
        _hsv_region->_label_separator_box->push_back(_hsv_region->_separator);
        _hsv_region->_main->push_back(_hsv_region->_label_separator_box);

        _rgb_region = new SliderRegion {
                new Label("RGB"),
                new SeparatorLine(),
                new Box(GTK_ORIENTATION_HORIZONTAL),
                new Box(GTK_ORIENTATION_VERTICAL)
        };

        _rgb_region->_label_separator_box->push_back(_rgb_region->_label);
        _rgb_region->_label_separator_box->push_back(_rgb_region->_separator);
        _rgb_region->_main->push_back(_rgb_region->_label_separator_box);

        static float slider_double_indent = 2 * state::margin_unit;

        for (char c : {'A', 'H', 'S', 'V', 'R', 'G', 'B'})
        {
            _sliders[c] = new Slider{
                new Shader(),
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                new GLArea(),
                new SpinButton(0, 1, 0.001),
                new Box(GTK_ORIENTATION_HORIZONTAL),
                new Vector2f(1, 1)
            };

            if (c == 'H')
            {
                _sliders[c]->_bar_shader->create_from_file(
                    get_resource_path() + "shaders/verbose_color_picker_hue_gradient.frag",
                    ShaderType::FRAGMENT
                );
            }

            SliderTuple_t* which = new std::tuple(c, this); // intentional memory leak

            _sliders[c]->_canvas->connect_signal("realize", on_slider_realize, which);
            _sliders[c]->_canvas->connect_signal("resize", on_slider_resize, which);
            _sliders[c]->_canvas->set_expand(true);

            _sliders[c]->_spin_button->set_expand(false);
            _sliders[c]->_spin_button->set_digits(3);
            //_sliders[c]->_spin_button->set_width_chars(3 + 2);
            _sliders[c]->_spin_button->set_halign(GTK_ALIGN_END);
            _sliders[c]->_spin_button->set_margin_start(0.75 * state::margin_unit);

            _sliders[c]->_spin_button->connect_signal("value-changed", on_slider_spin_button_value_changed, new std::tuple(c, this));

            _sliders[c]->_main->push_back(_sliders[c]->_canvas);
            _sliders[c]->_main->push_back(_sliders[c]->_spin_button);
            _sliders[c]->_main->set_margin(0.25 * state::margin_unit);
            _sliders[c]->_main->set_margin_start(slider_double_indent);
        }

        _sliders['H']->_spin_button->set_wrap(true);

        _opacity_region->_main->push_back(_sliders['A']->_main);

        _hsv_region->_main->push_back(_sliders['H']->_main);
        _hsv_region->_main->push_back(_sliders['S']->_main);
        _hsv_region->_main->push_back(_sliders['V']->_main);

        _rgb_region->_main->push_back(_sliders['R']->_main);
        _rgb_region->_main->push_back(_sliders['G']->_main);
        _rgb_region->_main->push_back(_sliders['B']->_main);

        Slider::_transparency_shader = new Shader();
        Slider::_transparency_shader->create_from_file(
            get_resource_path() + "shaders/transparency_tiling.frag",
            ShaderType::FRAGMENT
        );

        _html_region = new HtmlRegion{
            new Box(GTK_ORIENTATION_HORIZONTAL),
            new Label("HTML Color Code"),
            new SeparatorLine(),
            new Box(GTK_ORIENTATION_HORIZONTAL),
            new Entry(),
            new SeparatorLine(),
            new Button(),
            new Box(GTK_ORIENTATION_VERTICAL)
        };

        _html_region->_entry->set_n_chars(1 + 6 + 2);
        _html_region->_entry->connect_signal("activate", on_entry_activate, this);

        _html_region->_entry_hbox->push_back(_html_region->_entry);
        _html_region->_entry_hbox->push_back(_html_region->_entry_to_button_separator);
        _html_region->_entry_hbox->push_back(_html_region->_button);
        _html_region->_entry_hbox->set_margin_top(0.25 * state::margin_unit);

        _html_region->_label_hbox->push_back(_html_region->_label);
        _html_region->_label_hbox->push_back(_html_region->_label_separator);
        _html_region->_label_hbox->set_margin_bottom(0.25 * state::margin_unit);

        _html_region->_main->push_back(_html_region->_label_hbox);
        _html_region->_main->push_back(_html_region->_entry_hbox);

        _html_region->_entry->set_margin_start(slider_double_indent);
        _html_region->_entry->set_margin_top(0.25 * state::margin_unit);
        _html_region->_entry->set_halign(GTK_ALIGN_START);
        _html_region->_entry->set_margin_end(0.5 * state::margin_unit);

        _html_region->_button->set_halign(GTK_ALIGN_END);
        _html_region->_button->set_margin_start(0.5 * state::margin_unit);
        _html_region->_button->set_margin_end(state::margin_unit);

        _html_region->_entry_to_button_separator->set_hexpand(true);

        _current_color_region = new CurrentColorRegion{
            new GLArea(),
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            new Vector2f(1, 1),
            new Box(GTK_ORIENTATION_VERTICAL)
        };

        static float current_color_height = 3 * state::margin_unit;

        _current_color_region->_canvas->connect_signal("realize", on_current_color_region_realize, this);
        _current_color_region->_canvas->connect_signal("resize", on_current_color_region_resize, this);
        _current_color_region->_canvas->set_expand(true);
        _current_color_region->_canvas->set_size_request({1, current_color_height});
        _current_color_region->_canvas->set_valign(GTK_ALIGN_START);
        _current_color_region->_canvas->set_margin_start(8 * state::margin_unit);

        _current_color_region->_main->push_back(_current_color_region->_canvas);
        _current_color_region->_main->set_margin_top(0.1 * state::margin_unit);

        _all_regions_box = new Box(GTK_ORIENTATION_VERTICAL, 0.1 * state::margin_unit);
        _all_regions_box->push_back(_opacity_region->_main);
        _all_regions_box->push_back(_hsv_region->_main);
        _all_regions_box->push_back(_rgb_region->_main);
        _all_regions_box->push_back(_html_region->_main);

        static float regions_bottom_margin = 0.5 * state::margin_unit;

        _opacity_region->_main->set_margin_bottom(regions_bottom_margin);
        _hsv_region->_main->set_margin_bottom(regions_bottom_margin);
        _rgb_region->_main->set_margin_bottom(regions_bottom_margin);

        _all_regions_box->set_margin_top(0.5 * current_color_height + state::margin_unit);
        _all_regions_box->set_margin_bottom(state::margin_unit);

        _main = new Overlay();
        _main->add_overlay(_all_regions_box);
        _main->set_child(_current_color_region->_main);
    }

    VerboseColorPicker::operator GtkWidget*()
    {
        return _main->operator GtkWidget*();
    }

    VerboseColorPicker::~VerboseColorPicker()
    {
        delete _all_regions_box;
        delete _main;
        delete _opacity_region;
        delete _hsv_region;
        delete _rgb_region;

        for (auto& pair : _sliders)
            delete pair.second;

        delete _html_region;
        delete _current_color_region;
    }

    void VerboseColorPicker::on_slider_realize(GtkGLArea* area, SliderTuple_t* char_and_instance)
    {
        gtk_gl_area_make_current(area);

        char c = std::get<0>(*char_and_instance);
        auto* instance = std::get<1>(*char_and_instance);
        auto& self = instance->_sliders.at(c);

        self->_cursor = new Shape();
        self->_cursor->as_rectangle({0.5, 0}, {self->_width_to_cursor_width_ratio, 1});

        {
            auto size = self->_cursor->get_size();
            size.y -= 0.001;
            auto top_left = self->_cursor->get_top_left();

            std::vector<Vector2f> vertices = {
                    {top_left.x , top_left.y},
                    {top_left.x + size.x, top_left.y},
                    {top_left.x + size.x, top_left.y + size.y},
                    {top_left.x, top_left.y + size.y}
            };

            self->_cursor_frame = new Shape();
            self->_cursor_frame->as_wireframe(vertices);
            self->_cursor_frame->set_color(RGBA(0, 0, 0, 1));
        }

        static float bar_margin = 0.15;

        self->_bar = new Shape();
        self->_bar->as_rectangle({0, bar_margin}, {1, 1 - 2 * bar_margin});

        {
            auto size = self->_bar->get_size();
            auto top_left = self->_bar->get_top_left();
            size.x -= 0.0002;
            top_left.x += 0.0001;

            std::vector<Vector2f> vertices = {
                {top_left.x , top_left.y},
                {top_left.x + size.x, top_left.y},
                {top_left.x + size.x, top_left.y + size.y},
                {top_left.x, top_left.y + size.y}
            };

            self->_bar_frame = new Shape();
            self->_bar_frame->as_wireframe(vertices);
            self->_bar_frame->set_color(RGBA(0, 0, 0, 1));
        }

        self->_transparency_background = new Shape();
        self->_transparency_background->as_rectangle(self->_bar->get_top_left(), self->_bar->get_size());
        auto transparency_render_task = RenderTask(self->_transparency_background, self->_transparency_shader);
        transparency_render_task.register_vec2("_canvas_size", self->_canvas_size);

        self->_canvas->add_render_task(transparency_render_task);

        auto bar_render_task = RenderTask(self->_bar, self->_bar_shader);
        bar_render_task.register_vec2("_canvas_size", self->_canvas_size);
        bar_render_task.register_color("_current_color_hsva", &Slider::_hue_gradient_shader_current_color);

        self->_canvas->add_render_task(bar_render_task);
        self->_canvas->add_render_task(self->_bar_frame);
        self->_canvas->add_render_task(self->_cursor);
        self->_canvas->add_render_task(self->_cursor_frame);

        instance->update();
        gtk_gl_area_queue_render(area);
    }

    void VerboseColorPicker::on_slider_resize(GtkGLArea* area, int w, int h, SliderTuple_t* char_and_instance)
    {
        gtk_gl_area_make_current(area);

        char c = std::get<0>(*char_and_instance);
        auto* instance = std::get<1>(*char_and_instance);
        auto& self = instance->_sliders.at(c);

        float width = (state::margin_unit * 0.75) / w;

        self->_cursor->as_rectangle({0.5, 0}, {width, 1});

        {
            auto size = self->_cursor->get_size();
            size.y -= 0.001;
            auto top_left = self->_cursor->get_top_left();

            std::vector<Vector2f> vertices = {
                    {top_left.x , top_left.y},
                    {top_left.x + size.x, top_left.y},
                    {top_left.x + size.x, top_left.y + size.y},
                    {top_left.x, top_left.y + size.y}
            };

            self->_cursor_frame->as_wireframe(vertices);
        }

        self->set_value(self->value);

        self->_canvas_size->x = w;
        self->_canvas_size->y = h;

        gtk_gl_area_queue_render(area);
    }

    void VerboseColorPicker::on_current_color_region_realize(GtkGLArea* area, VerboseColorPicker* instance)
    {
        gtk_gl_area_make_current(area);

        auto* self = instance->_current_color_region;

        self->_transparency_background = new Shape();
        self->_transparency_background->as_rectangle({0, 0}, {1, 1});

        self->_last_color_shape = new Shape();
        self->_last_color_shape->as_rectangle({0, 0}, {self->_width_to_last_color_width_ration, 1});
        self->_last_color_shape->set_color(state::primary_color);

        self->_current_color_shape = new Shape();
        self->_current_color_shape->as_rectangle({self->_width_to_last_color_width_ration, 0}, {1 - self->_width_to_last_color_width_ration, 1});
        self->_current_color_shape->set_color(state::primary_color);

        {
            auto size = self->_transparency_background->get_size();
            size.y -= 0.0001;
            auto top_left = self->_transparency_background->get_top_left();
            top_left.x += 0.0001;

            std::vector<Vector2f> vertices = {
                    {top_left.x , top_left.y},
                    {top_left.x + size.x, top_left.y},
                    {top_left.x + size.x, top_left.y + size.y},
                    {top_left.x, top_left.y + size.y}
            };

            self->_frame = new Shape();
            self->_frame->as_wireframe(vertices);
            self->_frame->set_color(RGBA(0, 0, 0, 1));
        }

        self->_transparency_background = new Shape();
        self->_transparency_background->as_rectangle({0, 0}, {1, 1});
        self->_transparency_shader = new Shader();

        self->_transparency_shader = new Shader();
        self->_transparency_shader->create_from_file(
                get_resource_path() + "shaders/transparency_tiling.frag",
                ShaderType::FRAGMENT
        );

        auto transparency_render_task = RenderTask(self->_transparency_background, self->_transparency_shader);
        transparency_render_task.register_vec2("_canvas_size", self->_canvas_size);

        self->_canvas->add_render_task(transparency_render_task);
        self->_canvas->add_render_task(self->_current_color_shape);
        self->_canvas->add_render_task(self->_last_color_shape);
        self->_canvas->add_render_task(self->_frame);

        instance->update();
        gtk_gl_area_queue_render(area);
    }

    void VerboseColorPicker::on_current_color_region_resize(GtkGLArea* area, int w, int h, VerboseColorPicker* instance)
    {
        auto* self = instance->_current_color_region;
        self->_canvas_size->x = w;
        self->_canvas_size->y = h;

        self->_last_color_shape->set_top_left({0, 0}); //TODO: find vertex array bug that makes this necessary
    }

    HSVA VerboseColorPicker::get_color_with_component_set_to(HSVA color, char c, float value)
    {
        float h_before = color.h;

        if (c == 'A')
            color.a = value;
        else if (c == 'H')
            color.h = value;
        else if (c == 'S')
            color.s = value;
        else if (c == 'V')
            color.v = value;
        else
        {
            auto as_rgba = color.operator RGBA();
            if (c == 'R')
                as_rgba.r = value;
            else if (c == 'G')
                as_rgba.g = value;
            else if (c == 'B')
                as_rgba.b = value;

            color = as_rgba.operator HSVA();
            if (color.s == 0)
                color.h = h_before;
        }

        return color;
    }

    void VerboseColorPicker::on_slider_spin_button_value_changed(GtkSpinButton* button, SliderTuple_t* char_and_instance)
    {
        char c = std::get<0>(*char_and_instance);
        VerboseColorPicker* instance = std::get<1>(*char_and_instance);

        state::primary_color = get_color_with_component_set_to(state::primary_color, c, gtk_spin_button_get_value(button));
        // TODO: update others
        instance->update();
    }

    void VerboseColorPicker::on_entry_activate(GtkEntry* entry, VerboseColorPicker* instance)
    {
        std::string text = instance->sanitize_html_code(gtk_entry_buffer_get_text(gtk_entry_get_buffer(entry)));
        if (instance->is_html_code_valid(text))
        {
            auto color = instance->html_code_to_color(text);
            color.a = state::primary_color.a;

            if (color == state::primary_color)
                return;

            state::primary_color = color.operator HSVA();
            // TODO: update others
            instance->update(color.operator HSVA());
        }
    }

    void VerboseColorPicker::on_entry_paste(GtkEntry* entry, VerboseColorPicker* instance)
    {
        auto* buffer = gtk_entry_get_buffer(entry);
        std::string text = gtk_entry_buffer_get_text(buffer);
        text = instance->sanitize_html_code(text);
        gtk_entry_buffer_set_text(buffer, text.c_str(), text.size());
    }

    void VerboseColorPicker::Slider::set_value(float x)
    {
        if (_cursor == nullptr or _cursor_frame == nullptr)
            return;

        value = x;

        if (value < 0.001)
            value = 0;

        if (value > 1 - 0.001)
            value = 1;

        Vector2f centroid = {value, 0.5};

        if (centroid.x < _cursor->get_size().x * 0.5)
            centroid.x = _cursor->get_size().x * 0.5;

        if (centroid.x > 1 - _cursor->get_size().x * 0.5)
            centroid.x = 1 - _cursor->get_size().x * 0.5;

        // align to pixelgrid
        //centroid.x -= std::fmod(centroid.x, 1.f / _canvas_size->x);

        _cursor->set_centroid(centroid);
        _cursor_frame->set_centroid(centroid);

        _spin_button->set_all_signals_blocked(true);
        _spin_button->set_value(x);
        _spin_button->update();
        _spin_button->set_all_signals_blocked(false);
    }

    void VerboseColorPicker::update()
    {
        update(state::primary_color);
    }

    void VerboseColorPicker::update(HSVA color)
    {
        Slider::_hue_gradient_shader_current_color = color;

        auto set_left_right_color = [](Shape* shape, RGBA color_left, RGBA color_right)
        {
            if (shape == nullptr)
                return;

            shape->set_vertex_color(0, color_left);
            shape->set_vertex_color(1, color_right);
            shape->set_vertex_color(2, color_right);
            shape->set_vertex_color(3, color_left);
        };

        auto* slider = _sliders.at('A');
        slider->set_value(color.a);
        set_left_right_color(
            slider->_bar,
            HSVA(color.h, color.s, color.v, 0),
            HSVA(color.h, color.s, color.v, 1)
        );
        slider->_canvas->queue_render();

        slider = _sliders.at('H');
        slider->set_value(color.h);
        set_left_right_color(
                slider->_bar,
                HSVA(0, color.s, color.v, color.a),
                HSVA(1, color.s, color.v, color.a)
        );
        slider->_canvas->queue_render();

        slider = _sliders.at('S');
        slider->set_value(color.s);
        set_left_right_color(
                slider->_bar,
                HSVA(color.h, 0, color.v, color.a),
                HSVA(color.h, 1, color.v, color.a)
        );
        slider->_canvas->queue_render();

        slider = _sliders.at('V');
        slider->set_value(color.v);
        set_left_right_color(
                slider->_bar,
                HSVA(color.h, color.s, 0, color.a),
                HSVA(color.h, color.s, 1, color.a)
        );
        slider->_canvas->queue_render();

        auto as_rgba = color.operator RGBA();

        slider = _sliders.at('R');
        slider->set_value(as_rgba.r);
        set_left_right_color(
                slider->_bar,
                RGBA(0, as_rgba.g, as_rgba.b, as_rgba.a),
                RGBA(1, as_rgba.g, as_rgba.b, as_rgba.a)
        );
        slider->_canvas->queue_render();

        slider = _sliders.at('G');
        slider->set_value(as_rgba.g);
        set_left_right_color(
                slider->_bar,
                RGBA(as_rgba.r, 0, as_rgba.b, as_rgba.a),
                RGBA(as_rgba.r, 1, as_rgba.b, as_rgba.a)
        );
        slider->_canvas->queue_render();

        slider = _sliders.at('B');
        slider->set_value(as_rgba.b);
        set_left_right_color(
                slider->_bar,
                RGBA(as_rgba.r, as_rgba.g, 0, as_rgba.a),
                RGBA(as_rgba.r, as_rgba.g, 1, as_rgba.a)
        );
        slider->_canvas->queue_render();

        if (_current_color_region->_current_color_shape != nullptr)
            _current_color_region->_current_color_shape->set_color(color);

        if (_current_color_region->_last_color_shape != nullptr)
        {
            _current_color_region->_last_color_shape->set_color(color);
        }

        _current_color_region->_canvas->queue_render();

        _html_region->_entry->set_all_signals_blocked(true);
        _html_region->_entry->set_text(color_to_html_code(color.operator RGBA()));
        _html_region->_entry->set_all_signals_blocked(false);
    }

    std::string VerboseColorPicker::sanitize_html_code(const std::string& in)
    {
        std::string text;

        if (text.front() != '#')
            text.push_back('#');

        for (size_t i = 1; i < in.size(); ++i)
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

            text.push_back(c);
        }

        return text;
    }

    bool VerboseColorPicker::is_html_code_valid(const std::string& in)
    {
        if (in.front() != '#')
            return false;

        if (in.size() != 7)
            return false;

        static std::set<char> valid = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

        for (size_t i = 1; i < in.size(); ++i)
            if (valid.find(in.at(i)) == valid.end())
                return false;

        return true;
    }

    RGBA VerboseColorPicker::html_code_to_color(const std::string& in)
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

        auto text = sanitize_html_code(in);
        assert(is_html_code_valid(text));

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
        std::cerr << "[LOG] Unable to parse hex html code \"" << in << "\", returning RGBA(0, 0, 0, 1)" << std::endl;
        return RGBA(0, 0, 0, 1);
    }

    std::string VerboseColorPicker::color_to_html_code(RGBA in)
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

        return sanitize_html_code("#" + r_string + g_string + b_string);
    }

    /*

    void VerboseColorPicker::on_slider_button_press_event(GtkWidget* widget, GdkEventButton* event, SliderTuple_t* component_and_instance)
    {
        char c = std::get<0>(*component_and_instance);
        auto* instance = std::get<1>(*component_and_instance);
        auto* slider = instance->_sliders.at(c);

        auto pos = Vector2f{event->x, event->y};
        pos.x /= slider->_canvas_size->x;
        pos.y /= slider->_canvas_size->y;

        if (event->button == 1)
        {
            slider->_drag_active = true;
            auto value = glm::clamp<float>(pos.x, 0, 1);
            slider->set_value(value);
            auto color = get_color_with_component_set_to(state::primary_color, c, value);
            instance->update(color);
            instance->_current_color_region->_last_color_shape->set_color(state::primary_color);
        }
    }

    void VerboseColorPicker::on_slider_button_release_event(GtkWidget* widget, GdkEventButton* event,
                                                            SliderTuple_t* component_and_instance)
    {
        char c = std::get<0>(*component_and_instance);
        auto* instance = std::get<1>(*component_and_instance);
        auto* slider = instance->_sliders.at(c);

        auto pos = Vector2f{event->x, event->y};
        pos.x /= slider->_canvas_size->x;
        pos.y /= slider->_canvas_size->y;

        if (slider->_drag_active)
        {
            auto value = glm::clamp<float>(pos.x, 0, 1);
            auto color = state::primary_color;

            set_primary_color(get_color_with_component_set_to(color, c, value));
            instance->update();
            slider->_drag_active = false;
        }
    }

    void VerboseColorPicker::on_slider_motion_notify_event(GtkWidget* widget, GdkEventMotion* event, SliderTuple_t* component_and_instance)
    {
        char c = std::get<0>(*component_and_instance);
        auto* instance = std::get<1>(*component_and_instance);
        auto* slider = instance->_sliders.at(c);

        if (slider->_drag_active)
        {
            auto pos = Vector2f(event->x, event->y);
            pos.x /= slider->_canvas_size->x;
            pos.y /= slider->_canvas_size->y;
            auto value = glm::clamp<float>(pos.x, 0, 1);

            slider->set_value(value);
            auto color = get_color_with_component_set_to(state::primary_color, c, value);
            instance->update(color);
            instance->_current_color_region->_last_color_shape->set_color(state::primary_color);
        }
    }
     */
}