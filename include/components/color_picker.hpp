// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/colors.hpp>
#include <include/box.hpp>
#include <include/overlay.hpp>
#include <include/shape.hpp>
#include <include/gl_area.hpp>
#include <include/shader.hpp>
#include <include/get_resource_path.hpp>
#include <include/vector.hpp>
#include <include/entry.hpp>
#include <include/scale.hpp>
#include <include/button.hpp>

namespace mousetrap
{
    static inline HSVA current_color = HSVA(0.3, 1, 1, 1);
    static inline HSVA last_color = HSVA(0.2, 0.8, 1, 1);

    struct ColorPicker : public Widget
    {
        public:
            ColorPicker(float width);
            GtkWidget* get_native();

        private:
            // update

            static void update_color(ColorPicker* instance, char which_component, float value);
            static void update_gui();

            // current color display

            struct CurrentColorArea : public GLArea
            {
                CurrentColorArea() = default;
                ~CurrentColorArea();

                void on_realize(GtkGLArea*) override;

                Shape* _last_color_shape;
                Shape* _current_color_shape;

                Shape* _transparency_tiling;
                Shader* _transparency_tiling_shader;

                Shape* _frame;
            };

            CurrentColorArea _current_color_area;

            // slider element

            struct Gradient : public GLArea
            {
                Gradient(const std::string& shader_path = "");
                ~Gradient();

                void on_realize(GtkGLArea*) override;

                Shape* _shape;
                Shader* _shader = nullptr;
            };

            struct SliderElement : public Widget
            {
                SliderElement(const std::string& shader_path = "");
                ~SliderElement();

                Gradient* _gradient;
                Scale* _scale;
                Box* _hbox;
                Entry* _entry;

                GtkWidget* get_native() override {
                    return _hbox->get_native();
                }
            };

            std::map<char, SliderElement*> _elements = {
                    {'A', nullptr},
                    {'H', nullptr},
                    {'S', nullptr},
                    {'V', nullptr},
                    {'R', nullptr},
                    {'G', nullptr},
                    {'B', nullptr},

            };

            // html code

            struct HtmlCodeElement : public Widget
            {
                HtmlCodeElement();
                ~HtmlCodeElement();

                Entry* _entry;
                Button* _button;
                Box* _hbox;

                GtkWidget* get_native() override {
                    return _hbox->get_native();
                }

                static std::string sanitize_html_code(const std::string&);
                static RGBA parse_html_code(const std::string&);
            };

            HtmlCodeElement _html_code_element;

            // global containers

            Box _opacity_region;
            Box _hsv_region;
            Box _rgb_region;
            Box _html_region;

            Box _all_slider_regions;
            Box _current_color_overlay;
            Box _close_dialogue_overlay;

            Box _window;

            // global config

            static inline float margin = 10;

            static inline const float left_to_slider_left_margin = 1.5 * margin;
            // left of window to left of slider
d
            static inline const float left_to_label_left_margin = 1 * margin;
            // left of window to left of label

            static inline const float outer_margin = 2 * margin;
            // around outermost box element

            static inline const float scale_to_entry_spacer = 1 * margin;
            // right of scale to left of entry for each slider

            static inline const float scale_to_entry_x_ratio = 0.8;
            // scale.width = factor * (box.width - scale_to_entry_spacer)
            // entry.width = (1 - factor) * (box.width - scale_to_entry_spacer)

            static inline const float slider_height = 1.5 * margin;
            // height of slider + gradient + entry

            static inline const float slider_to_slider_spacer = 0.5 * margin;
            // bottom of slider to top of next slider within same region (HSV, RGBA, CYMK)
            // also space from bottom of last slider in region to top of label of next region
            // also space from bottom of label to top of first slider in that region

            static inline const float last_color_to_current_color_x_ratio = 1 / 12.f;
            // width of "last color" = factor * width of current_color_area

            static inline const float current_color_y_height = 10 * margin;
            // height of last color

            static inline const float close_dialogue_height = 0.5 * margin;
            // closed dialogue = 3*h x h
    };
}

// ###

namespace mousetrap
{
    // current color area

    void ColorPicker::CurrentColorArea::on_realize(GtkGLArea* area)
    {
        gtk_gl_area_make_current(area);

        float last_width = last_color_to_current_color_x_ratio;

        _last_color_shape = new Shape();
        _last_color_shape->as_rectangle({0, 0}, {last_width, 1});
        _last_color_shape->set_color(last_color);

        _current_color_shape = new Shape();
        _current_color_shape->as_rectangle({last_width, 0}, {1 - last_width, 1});
        _current_color_shape->set_color(current_color);

        _transparency_tiling = new Shape();
        _transparency_tiling->as_rectangle({0, 0}, {1, 1});

        _transparency_tiling_shader = new Shader();
        _transparency_tiling_shader->create_from_file(get_resource_path() + "shaders/transparency_tiling.frag", ShaderType::FRAGMENT);

        auto size = get_size_request();

        _frame = new Shape();
        float target_frame_size = 0.025;
        Vector2f frame_size = Vector2f(target_frame_size * (size.y / size.x), target_frame_size);
        _frame->as_frame({0, 0}, {1, 1}, frame_size.x, frame_size.y);
        _frame->set_color(RGBA(0, 0, 0, 1));

        add_render_task(_transparency_tiling, _transparency_tiling_shader);
        add_render_task(_current_color_shape);
        add_render_task(_last_color_shape);
        add_render_task(_frame);
    }

    ColorPicker::CurrentColorArea::~CurrentColorArea()
    {
        delete _last_color_shape;
        delete _current_color_shape;
        delete _transparency_tiling;
        delete _transparency_tiling_shader;
        delete _frame;
    }

    // slider element

    ColorPicker::Gradient::Gradient(const std::string& shader_path)
    {
        _shader = new Shader();
        if (not shader_path.empty())
            _shader->create_from_file(shader_path, ShaderType::FRAGMENT);
    }

    void ColorPicker::Gradient::on_realize(GtkGLArea* area)
    {
        gtk_gl_area_make_current(area);
        _shape = new Shape();
        _shape->as_rectangle({0, 0}, {1, 1});

        add_render_task(_shape, _shader);
    }

    ColorPicker::Gradient::~Gradient()
    {
        delete _shader;
        delete _shape;
    }

    ColorPicker::SliderElement::SliderElement(const std::string& shader_path)
    {
        _gradient = new ColorPicker::Gradient(shader_path);
        TODO
    }

    ColorPicker::SliderElement::~SliderElement()
    {
        delete _gradient;
        delete _scale;
        delete _hbox;
        delete _entry;
    }

    GtkWidget* ColorPicker::get_native()
    {
        return _window.get_native();
    }
}