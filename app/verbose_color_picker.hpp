// 
// Copyright 2022 Clemens Cords
// Created on 8/17/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

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

namespace mousetrap
{
    class VerboseColorPicker : public Widget
    {
        public:
            VerboseColorPicker();
            ~VerboseColorPicker();

            GtkWidget* get_native() override;

            void update() override;

        private:
            Box* _all_regions_box;
            Overlay* _main;

            // slider regions

            struct SliderRegion
            {
                Label* _label;
                GtkSeparatorMenuItem* _separator;
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

                static inline float _width_to_slider_width_ratio = 0.05;

                ~Slider() {
                    delete _bar_shader;
                    delete _cursor;
                    delete _cursor_frame;
                    delete _bar;
                    delete _bar_frame;
                    delete _transparency_background;
                    delete _canvas;
                    delete _spin_button;
                    delete _main;
                    delete _canvas_size;
                }
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

            using SliderTuple_t = std::tuple<char, VerboseColorPicker*>;
            static void on_slider_realize(GtkGLArea* area, SliderTuple_t* char_and_instance);
            static void on_slider_resize(GtkGLArea* area, int w, int h, SliderTuple_t* char_and_instance);
            static void on_slider_spin_button_value_changed(GtkSpinButton*, SliderTuple_t* char_and_instance);

            // html region

            struct HtmlRegion
            {
                Box* _main;
                Label* _label;
                Entry* _entry;
                Button* _button; // TODO
            };

            HtmlRegion* _html_region;
            static void on_entry_activate(GtkEntry*, VerboseColorPicker* instance);
            static void on_entry_paste(GtkEntry*, VerboseColorPicker* instance);

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
                    delete _canvas;
                    delete _current_color_shape;
                    delete _last_color_shape;
                    delete _frame;
                    delete _transparency_background;
                    delete _canvas_size;
                    delete _main;
                }
            };

            CurrentColorRegion* _current_color_region;

            static void on_current_color_region_realize(GtkGLArea* area, VerboseColorPicker* instance);
            static void on_current_color_region_resize(GtkGLArea* area, int w, int h, VerboseColorPicker* instance);
    };

    namespace state
    {
        static inline VerboseColorPicker* verbose_color_picker_instance = nullptr;
    }
}

// ###

namespace mousetrap
{
    VerboseColorPicker::VerboseColorPicker()
    {
        _opacity_region = new SliderRegion {
            new Label("Opacity"),
            GTK_SEPARATOR_MENU_ITEM(gtk_separator_menu_item_new()),
            new Box(GTK_ORIENTATION_HORIZONTAL),
            new Box(GTK_ORIENTATION_VERTICAL)
        };

        _opacity_region->_label_separator_box->add(_opacity_region->_label);
        _opacity_region->_label_separator_box->add((GtkWidget*) _opacity_region->_separator);
        _opacity_region->_main->add(_opacity_region->_label_separator_box);

        _hsv_region = new SliderRegion {
                new Label("HSV"),
                GTK_SEPARATOR_MENU_ITEM(gtk_separator_menu_item_new()),
                new Box(GTK_ORIENTATION_HORIZONTAL),
                new Box(GTK_ORIENTATION_VERTICAL)
        };

        _hsv_region->_label_separator_box->add(_hsv_region->_label);
        _hsv_region->_label_separator_box->add((GtkWidget*) _hsv_region->_separator);
        _hsv_region->_main->add(_hsv_region->_label_separator_box);

        _rgb_region = new SliderRegion {
                new Label("RGB"),
                GTK_SEPARATOR_MENU_ITEM(gtk_separator_menu_item_new()),
                new Box(GTK_ORIENTATION_HORIZONTAL),
                new Box(GTK_ORIENTATION_VERTICAL)
        };

        _rgb_region->_label_separator_box->add(_rgb_region->_label);
        _rgb_region->_label_separator_box->add((GtkWidget*) _rgb_region->_separator);
        _rgb_region->_main->add(_rgb_region->_label_separator_box);

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
                new Box(GTK_ORIENTATION_VERTICAL),
                new Vector2f(1, 1)
            };

            if (c == 'H')
            {
                _sliders[c]->_bar_shader->create_from_file(
                    get_resource_path() + "shaders/color_picker_hue_gradient.frag",
                    ShaderType::FRAGMENT
                );
            }

            _sliders[c]->_canvas->connect_signal("realize", on_slider_realize, new std::tuple(c, this));
            _sliders[c]->_canvas->connect_signal("resize", on_slider_resize, new std::tuple(c, this));

            _sliders[c]->_main->add(_sliders[c]->_canvas);
            _sliders[c]->_main->add(_sliders[c]->_spin_button);

            _sliders[c]->_spin_button->set_expand(false);
            _sliders[c]->_spin_button->set_digits(3);
            _sliders[c]->_spin_button->set_width_chars(2 * 3);
            _sliders[c]->_spin_button->set_halign(GTK_ALIGN_END);
        }

        _opacity_region->_main->add(_sliders['A']->_main);

        _hsv_region->_main->add(_sliders['H']->_main);
        _hsv_region->_main->add(_sliders['S']->_main);
        _hsv_region->_main->add(_sliders['V']->_main);

        _rgb_region->_main->add(_sliders['R']->_main);
        _rgb_region->_main->add(_sliders['G']->_main);
        _rgb_region->_main->add(_sliders['B']->_main);

        Slider::_transparency_shader = new Shader();
        Slider::_transparency_shader->create_from_file(
            get_resource_path() + "shaders/transparency_tiling.frag",
            ShaderType::FRAGMENT
        );

        _html_region = new HtmlRegion{
            new Box(GTK_ORIENTATION_HORIZONTAL),
            new Label("HTML Code:"),
            new Entry(),
            new Button()
        };

        _html_region->_entry->connect_signal("activate", on_entry_activate, this);
        _html_region->_entry->connect_signal("paste-clipboard", on_entry_paste, this);

        _html_region->_main->add(_html_region->_label);
        _html_region->_main->add(_html_region->_entry);
        _html_region->_main->add(_html_region->_button);

        _current_color_region = new CurrentColorRegion{
            new GLArea(),
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            new Vector2f(1, 1),
            new Box(GTK_ORIENTATION_VERTICAL)
        };

        _current_color_region->_canvas->connect_signal("realize", on_current_color_region_realize, this);
        _current_color_region->_canvas->connect_signal("resize", on_current_color_region_resize, this);
        _current_color_region->_main->add(_current_color_region->_canvas);

        _all_regions_box = new Box(GTK_ORIENTATION_VERTICAL);
        _all_regions_box->add(_opacity_region->_main);
        _all_regions_box->add(_hsv_region->_main);
        _all_regions_box->add(_rgb_region->_main);
        _all_regions_box->add(_html_region->_main);

        _main = new Overlay();
        _main->set_under(_all_regions_box);
        _main->set_over(_current_color_region->_main);
    }

    GtkWidget* VerboseColorPicker::get_native()
    {
        return _main->get_native();
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
        self->_cursor->as_rectangle({0.5, 0}, {self->_width_to_slider_width_ratio, 1});

        {
            auto size = self->_cursor->get_size() - Vector2f(0.01);
            auto top_left = self->_cursor->get_size();

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

        self->_bar = new Shape();
        self->_bar->as_rectangle({0, 0}, {1, 1});

        {
            auto size = self->_bar->get_size() - Vector2f(0.01);
            auto top_left = self->_bar->get_size();

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
        self->_transparency_background->as_rectangle({0, 0}, {1, 1});
        auto transparency_render_task = RenderTask(self->_transparency_background, self->_transparency_shader);
        transparency_render_task.register_vec2("_canvas_size", self->_canvas_size);

        self->_canvas->add_render_task(transparency_render_task);
        self->_canvas->add_render_task(self->_bar, self->_bar_shader);
        self->_canvas->add_render_task(self->_bar_frame);
        self->_canvas->add_render_task(self->_cursor);
        self->_canvas->add_render_task(self->_cursor_frame);

        gtk_gl_area_queue_render(area);
    }

    void VerboseColorPicker::on_slider_resize(GtkGLArea* area, int w, int h, SliderTuple_t* char_and_instance)
    {
        return;
        gtk_gl_area_make_current(area);

        char c = std::get<0>(*char_and_instance);
        auto* instance = std::get<1>(*char_and_instance);
        auto& self = instance->_sliders.at(c);

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

        self->_current_color_shape = new Shape();
        self->_current_color_shape->as_rectangle({self->_width_to_last_color_width_ration, 0}, {1 - self->_width_to_last_color_width_ration, 1});

        {
            std::vector<Vector2f> vertices;
            for (size_t i = 0; i < self->_transparency_background->get_n_vertices(); ++i)
                vertices.push_back(self->_transparency_background->get_vertex_position(i));

            self->_frame = new Shape();
            self->_frame->as_wireframe(vertices);
            self->_frame->set_color(RGBA(0, 0, 0, 1));
        }

        self->_transparency_background->as_rectangle({0, 0}, {1, 1});
        auto transparency_render_task = RenderTask(self->_transparency_background, self->_transparency_shader);
        transparency_render_task.register_vec2("_canvas_size", self->_canvas_size);

        self->_canvas->add_render_task(transparency_render_task);
        self->_canvas->add_render_task(self->_current_color_shape);
        self->_canvas->add_render_task(self->_last_color_shape);
        self->_canvas->add_render_task(self->_frame);
    }

    void VerboseColorPicker::on_current_color_region_resize(GtkGLArea* area, int w, int h, VerboseColorPicker* instance)
    {
        auto* self = instance->_current_color_region;
        self->_canvas_size->x = w;
        self->_canvas_size->y = h;
    }

    void VerboseColorPicker::on_slider_spin_button_value_changed(GtkSpinButton*, SliderTuple_t* char_and_instance)
    {}

    void VerboseColorPicker::on_entry_activate(GtkEntry*, VerboseColorPicker* instance)
    {}

    void VerboseColorPicker::on_entry_paste(GtkEntry*, VerboseColorPicker* instance)
    {}

    void VerboseColorPicker::update()
    {}
}