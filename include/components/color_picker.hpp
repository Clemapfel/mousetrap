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
#include <include/spin_button.hpp>
#include <include/label.hpp>

namespace mousetrap
{
    static inline HSVA current_color = HSVA(0.3, 1, 1, 1);
    static inline HSVA last_color = HSVA(0.2, 0.8, 1, 1);

    class ColorPicker;
    static inline ColorPicker* color_picker = nullptr;

    class ColorPicker : public Widget
    {
        public:
            ColorPicker(float width);
            virtual ~ColorPicker();

            GtkWidget* get_native();

        private:
            // update
            static void update_color(ColorPicker* instance, char which_component, float value);
            static void update_gui(ColorPicker* instance);

            // current color display

            struct CurrentColorArea : public GLArea
            {
                CurrentColorArea() = default;
                virtual ~CurrentColorArea();

                void on_realize(GtkGLArea*) override;
                void on_resize(GtkGLArea*, int, int) override;

                void update();

                Shape* _last_color_shape;
                Shape* _current_color_shape;

                Shape* _transparency_tiling;
                Shader* _transparency_tiling_shader;

                Shape* _frame;
            };

            CurrentColorArea* _current_color_area;

            // label

            struct LabelElement : public Widget
            {
                LabelElement(const std::string&);
                virtual ~LabelElement();

                Label* _label;
                Box* _hbox;
                GtkWidget* _separator;

                GtkWidget* get_native() {
                    return _hbox->get_native();
                }
            };

            LabelElement* _opacity_label;
            LabelElement* _hsv_label;
            LabelElement* _rgb_label;
            LabelElement* _html_label;

            // slider element

            struct Gradient : public GLArea
            {
                Gradient(const std::string& shader_path = "");
                virtual ~Gradient();

                void on_realize(GtkGLArea*) override;
                void on_resize(GtkGLArea*, int, int) override;

                Shape* _shape;
                Shader* _shader = nullptr;
            };

            struct SliderElement : public Widget
            {
                SliderElement(char component, const std::string& shader_path = "");
                virtual ~SliderElement();

                void update();

                Gradient* _gradient;
                Scale* _scale;
                SpinButton* _entry;

                Overlay* _overlay;
                Box* _hbox;

                char _component;

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
                virtual ~HtmlCodeElement();

                Entry* _entry;
                Button* _button;
                Box* _hbox;

                GtkWidget* get_native() override {
                    return _hbox->get_native();
                }

                static std::string sanitize_html_code(const std::string&);
                static RGBA parse_html_code(const std::string&);
            };

            HtmlCodeElement* _html_code_element;

            // closing dialogue placeholder

            struct ClosingDialogue : public Widget
            {
                ClosingDialogue();
                virtual ~ClosingDialogue();

                Button* _left;
                Button* _center;
                Button* _right;
                Box* _hbox;

                GtkWidget* get_native() override {
                    return _hbox->get_native();
                }
            };

            ClosingDialogue* _closing_dialogue;

            // global containers

            Box* _opacity_region;
            Box* _hsv_region;
            Box* _rgb_region;
            Box* _html_region;

            Box* _all_slider_regions;
            Overlay* _current_color_overlay;
            Overlay* _closing_dialogue_overlay;

            Box* _window;

            // global config

            static inline float margin = 10;

            static inline const float left_to_slider_left_margin = 1.5 * margin;
            // left of window to left of slider

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

        update();
    }

    void ColorPicker::CurrentColorArea::on_resize(GtkGLArea*, int, int)
    {
        std::cout << _current_color_shape << std::endl;
    }

    void ColorPicker::CurrentColorArea::update()
    {
        _last_color_shape->set_color(last_color);
        _current_color_shape->set_color(current_color);
        queue_render();
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

    void ColorPicker::Gradient::on_resize(GtkGLArea*, int w, int h)
    {
        std::cout << _shape << std::endl;
    }

    ColorPicker::Gradient::~Gradient()
    {
        delete _shader;
        delete _shape;
    }

    ColorPicker::SliderElement::SliderElement(char component, const std::string& shader_path)
    {
        _component = component;

        _scale = new Scale(0, 1, 0.001);
        _entry = new SpinButton(0, 1, 0.001);

        _scale->set_opacity(0.25);

        _gradient = new ColorPicker::Gradient(shader_path);

        _overlay = new Overlay();
        _overlay->set_under(_gradient);
        _overlay->set_over(_scale);

        _gradient->set_margin_top(0.5 * margin);
        _gradient->set_margin_bottom(0.5 * margin);
        _gradient->set_margin_start(0.25 * margin);
        _gradient->set_margin_end(0.25 * margin);

        _overlay->set_margin_start(left_to_slider_left_margin);
        _overlay->set_margin_end(scale_to_entry_spacer);

        _hbox = new Box(GTK_ORIENTATION_HORIZONTAL);
        _hbox->add(_overlay);
        _hbox->add(_entry);
    }

    void ColorPicker::SliderElement::update()
    {
        auto set_gradient_color = [&](RGBA left_color, RGBA right_color)
        {
            _gradient->_shape->set_vertex_color(0, left_color);
            _gradient->_shape->set_vertex_color(1, right_color);
            _gradient->_shape->set_vertex_color(2, right_color);
            _gradient->_shape->set_vertex_color(3, left_color);
        };

        auto rgba = current_color.operator RGBA();
        auto hsva = current_color;

        if (_component == 'A')
        {
            auto a_0 = rgba;
            a_0.a = 0;
            auto a_1 = rgba;
            a_1.a = 1;

            set_gradient_color(a_0, a_1);
            return;
        }

        hsva.a = 1;
        rgba.a = 1;

        if (_component == 'S')
        {
            auto s_0 = hsva;
            s_0.s = 0;
            auto s_1 = hsva;
            s_1.s = 1;

            set_gradient_color(s_0, s_1);
        }
        else if (_component == 'V')
        {
            auto v_0 = hsva;
            v_0.v = 0;
            auto v_1 = hsva;
            v_1.v = 1;

            set_gradient_color(v_0, v_1);
        }
        else if (_component == 'R')
        {
            auto r_0 = rgba;
            r_0.r = 0;
            auto r_1 = rgba;
            r_1.r = 1;

            set_gradient_color(r_0, r_1);
        }
        else if (_component == 'G')
        {
            auto g_0 = rgba;
            g_0.g = 0;
            auto g_1 = rgba;
            g_1.g = 1;

            set_gradient_color(g_0, g_1);
        }
        else if (_component == 'B')
        {
            auto b_0 = rgba;
            b_0.b = 0;
            auto b_1 = rgba;
            b_1.b = 1;

            set_gradient_color(b_0, b_1);
        }
    }

    ColorPicker::SliderElement::~SliderElement()
    {
        delete _gradient;
        delete _scale;
        delete _hbox;
        delete _entry;
    }

    ColorPicker::HtmlCodeElement::HtmlCodeElement()
    {
        _entry = new Entry();
        _entry->set_width_chars(4 * 2 + 1);
        _entry->set_expand_horizontally(true);
        _entry->set_margin_start(left_to_slider_left_margin);

        _button = new Button();
        _button->set_label("!");
        _button->set_margin_start(scale_to_entry_spacer);

        _hbox = new Box(GTK_ORIENTATION_HORIZONTAL, slider_to_slider_spacer);
        _hbox->add(_entry);
        _hbox->add(_button);
    }

    ColorPicker::HtmlCodeElement::~HtmlCodeElement()
    {
        delete _entry;
        delete _button;
        delete _hbox;
    }

    ColorPicker::LabelElement::LabelElement(const std::string& str)
    {
        _label = new Label(str);
        _hbox = new Box(GTK_ORIENTATION_HORIZONTAL, scale_to_entry_spacer);
        _separator = gtk_separator_menu_item_new();

        _hbox->add(_label);
        _hbox->add(_separator);
        _hbox->set_margin_start(left_to_label_left_margin);
    }

    ColorPicker::LabelElement::~LabelElement()
    {
        delete _hbox;
        delete _label;
        //gtk_widget_destroy(_separator);
    }

    ColorPicker::ClosingDialogue::ClosingDialogue()
    {
        _left = new Button();
        _center = new Button();
        _right = new Button();
        _hbox = new Box(GTK_ORIENTATION_HORIZONTAL);

        _hbox->add(_left);
        _hbox->add(_center);
        _hbox->add(_right);
    }

    ColorPicker::ClosingDialogue::~ClosingDialogue()
    {
        delete _left;
        delete _center;
        delete _right;
        delete _hbox;
    }

    GtkWidget* ColorPicker::get_native()
    {
        return _window->get_native();
    }

    ColorPicker::ColorPicker(float width)
    {
        _elements.at('H') = new SliderElement('H', get_resource_path() + "shaders/color_picker_hue_gradient.frag");

        for (char c : {'A', 'S', 'V', 'R', 'G', 'B'})
            _elements.at(c) = new SliderElement(c);

        for (auto& pair : _elements)
        {
            pair.second->_gradient->set_size_request({width, margin});
            pair.second->_gradient->set_expand_horizontally(true);
        }

        _opacity_label = new LabelElement("Opacity");
        _hsv_label = new LabelElement("HSV");
        _rgb_label = new LabelElement("RGB");
        _html_label = new LabelElement("Hexadecimal");

        _html_code_element = new HtmlCodeElement();

        _opacity_region = new Box(GTK_ORIENTATION_VERTICAL, slider_to_slider_spacer);
        _opacity_region->add(_opacity_label);
        _opacity_region->add(_elements.at('A'));

        _hsv_region = new Box(GTK_ORIENTATION_VERTICAL, slider_to_slider_spacer);
        _hsv_region->add(_hsv_label);
        _hsv_region->add(_elements.at('H'));
        _hsv_region->add(_elements.at('S'));
        _hsv_region->add(_elements.at('V'));

        _rgb_region = new Box(GTK_ORIENTATION_VERTICAL, slider_to_slider_spacer);
        _rgb_region->add(_rgb_label);
        _rgb_region->add(_elements.at('R'));
        _rgb_region->add(_elements.at('G'));
        _rgb_region->add(_elements.at('B'));

        _html_region = new Box(GTK_ORIENTATION_VERTICAL, slider_to_slider_spacer);
        _html_region->add(_html_label);
        _html_region->add(_html_code_element);

        _all_slider_regions = new Box(GTK_ORIENTATION_VERTICAL, margin);
        _all_slider_regions->add(_opacity_region);
        _all_slider_regions->add(_hsv_region);
        _all_slider_regions->add(_rgb_region);
        _all_slider_regions->add(_html_region);

        _current_color_area = new CurrentColorArea();
        _current_color_area->set_align_vertically(GtkAlign::GTK_ALIGN_START);
        _current_color_area->set_expand_vertically(false);
        _current_color_area->set_expand_horizontally(true);
        _current_color_area->set_size_request({1, 4 * margin});
        _current_color_area->set_margin_start(0.6 * width);

        _all_slider_regions->set_margin_top(_current_color_area->get_size_request().y - margin);

        _current_color_overlay = new Overlay();
        _current_color_overlay->set_under(_all_slider_regions);
        _current_color_overlay->set_over(_current_color_area);
        _current_color_overlay->set_margin_end(outer_margin);
        _current_color_overlay->set_margin_start(0.5 * outer_margin);
        _current_color_overlay->set_margin_top(0.5 * outer_margin);
        _current_color_overlay->set_margin_bottom(0.5 * outer_margin);

        _closing_dialogue = new ClosingDialogue();
        _closing_dialogue->set_align_horizontally(GtkAlign::GTK_ALIGN_START);
        _closing_dialogue->set_align_vertically(GtkAlign::GTK_ALIGN_START);

        _closing_dialogue_overlay = new Overlay();
        _closing_dialogue_overlay->set_under(_current_color_overlay);
        _closing_dialogue_overlay->set_over(_closing_dialogue);

        _window = new Box(GTK_ORIENTATION_VERTICAL);
        _closing_dialogue_overlay->set_align_horizontally(GtkAlign::GTK_ALIGN_CENTER);
        _closing_dialogue_overlay->set_align_vertically(GtkAlign::GTK_ALIGN_CENTER);

        _window->add(_closing_dialogue_overlay);
    }

    ColorPicker::~ColorPicker()
    {
        delete _current_color_area;

        delete _opacity_label;
        delete _hsv_label;
        delete _rgb_label;
        delete _html_label;

        for (auto& pair : _elements)
        {
            delete pair.second;
            pair.second = nullptr;
        }

        delete _html_code_element;
        delete _closing_dialogue;

        delete _opacity_region;
        delete _hsv_region;
        delete _rgb_region;
        delete _html_region;
        delete _all_slider_regions;
        delete _current_color_overlay;
        delete _closing_dialogue_overlay;
        delete _window;
    }

    void ColorPicker::update_gui(ColorPicker* instance)
    {
        for (auto& pair : instance->_elements)
            pair.second->update();

        instance->_current_color_area->update();
    }
}