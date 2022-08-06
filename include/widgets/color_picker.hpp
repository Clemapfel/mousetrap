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

#include <include/widgets/global_state.hpp>

#include <set>

namespace mousetrap
{
    class ColorPicker : public Widget, public Updatable
    {
        public:
            ColorPicker(float width);
            virtual ~ColorPicker();

            GtkWidget* get_native();
            void update() override;
            bool is_initialized() const override;
            
        private:
            static inline RGBA current_color_as_rgba = RGBA(1, 1, 1, 1);

            // update
            static void update_color(ColorPicker* instance, char which_component, float value);
            static void update_gui(ColorPicker* instance);
            static void connect_signals(ColorPicker* instance);

            // signal wrapper
            static void scale_value_changed(GtkRange* scale, std::pair<ColorPicker*, char>* instance_and_which);
            static void spin_button_value_changed(GtkSpinButton* button, std::pair<ColorPicker*, char>* instance_and_which);
            static void entry_on_activate(GtkEntry* entry, ColorPicker* instance);
            static void entry_on_paste(GtkEntry* entry, ColorPicker* instance);
            static gboolean widget_on_draw(void* , void*, ColorPicker* instance);

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
                Vector2f* _canvas_size;
            };

            CurrentColorArea* _current_color_area;

            // eyedropped

            struct EyedropperModeButton : public Widget
            {
                EyedropperModeButton();
                ~EyedropperModeButton();

                Button* _button;

                GtkWidget* get_native() {
                    return _button->get_native();
                }
            };

            EyedropperModeButton* _eyedropper_button;

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
                Shape* _frame;
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

                GtkWidget* get_native() override {
                    return _entry->get_native();
                }

                void update();

                static std::string sanitize_html_code(const std::string&);
                static bool is_html_code_valid(const std::string& sanitized);
                static RGBA code_to_color(const std::string&);
                static std::string color_to_code(RGBA);

                static void on_clicked(GtkButton*, ColorPicker* instance) {
                    // TODO: temp
                    instance->update_gui(instance);
                }
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
            Overlay* _current_color_and_eyedropper_overlay;
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

    ColorPicker* get_color_picker()
    {
        return (ColorPicker*) color_picker;
    }
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
        _last_color_shape->set_color(current_color);

        _current_color_shape = new Shape();
        _current_color_shape->as_rectangle({last_width, 0}, {1 - last_width, 1});
        _current_color_shape->set_color(current_color);

        _transparency_tiling = new Shape();
        _transparency_tiling->as_rectangle({0, 0}, {1, 1});

        _transparency_tiling_shader = new Shader();
        _transparency_tiling_shader->create_from_file(get_resource_path() + "shaders/transparency_tiling.frag", ShaderType::FRAGMENT);

        auto size = get_size();

        _frame = new Shape();
        float target_frame_size = 0.025;
        Vector2f frame_size = Vector2f(target_frame_size * (size.y / size.x), target_frame_size);
        _frame->as_frame({0, 0}, {1, 1}, frame_size.x, frame_size.y);
        _frame->set_color(RGBA(0, 0, 0, 1));

        _canvas_size = new Vector2f(size);
        auto tiling_task = RenderTask(_transparency_tiling, _transparency_tiling_shader);
        tiling_task.register_vec2("_canvas_size", _canvas_size);

        add_render_task(tiling_task);
        add_render_task(_current_color_shape);
        add_render_task(_last_color_shape);
        add_render_task(_frame);

        update();
    }

    void ColorPicker::CurrentColorArea::on_resize(GtkGLArea*, int width, int height)
    {
        if (width == 0 or height == 0)
            return;

        float target_frame_size = 1.f / height;
        Vector2f frame_size = Vector2f(target_frame_size / (width / float(height)), target_frame_size);
        _frame->as_frame({0, 0}, {1, 1}, frame_size.x, frame_size.y);
        _frame->set_color(RGBA(0, 0, 0, 1));

        _canvas_size->x = width;
        _canvas_size->y = height;
    }

    void ColorPicker::CurrentColorArea::update()
    {
        _last_color_shape->set_color(current_color);
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
        delete _canvas_size;
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

        auto size = get_size();

        _frame = new Shape();
        _frame->as_rectangle({0, 0}, {1, 1});
        on_resize(area, size.x, size.y);

        auto render_task = RenderTask(_shape, _shader, nullptr);
        render_task.register_color("_current_color_hsva", &current_color);
        render_task.register_float("_hue_offset", new float(0));

        add_render_task(render_task);
        add_render_task(_frame);
    }

    void ColorPicker::Gradient::on_resize(GtkGLArea*, int width, int height)
    {
        if (width == 0 or height == 0)
            return;

        float target_frame_size = 1.f / height;
        Vector2f frame_size = Vector2f(target_frame_size / (width / float(height)), target_frame_size);
        _frame->as_frame({0, 0}, {1, 1}, frame_size.x, frame_size.y);
        _frame->set_color(RGBA(0, 0, 0, 1));
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
        _entry->set_all_signals_blocked(true);
        _scale->set_all_signals_blocked(true);

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
            _entry->set_value(rgba.a);
            _scale->set_value(rgba.a);
            goto done;
        }

        hsva.a = 1;
        rgba.a = 1;

        if (_component == 'H')
        {
            _entry->set_value(hsva.h);
            _scale->set_value(hsva.h);
            goto done;
        }
        else if (_component == 'S')
        {
            auto s_0 = hsva;
            s_0.s = 0;
            auto s_1 = hsva;
            s_1.s = 1;

            set_gradient_color(s_0, s_1);
            _entry->set_value(hsva.s);
            _scale->set_value(hsva.s);
            goto done;
        }
        else if (_component == 'V')
        {
            auto v_0 = hsva;
            v_0.v = 0;
            auto v_1 = hsva;
            v_1.v = 1;

            set_gradient_color(v_0, v_1);
            _entry->set_value(hsva.v);
            _scale->set_value(hsva.v);
            goto done;
        }
        else if (_component == 'R')
        {
            auto r_0 = rgba;
            r_0.r = 0;
            auto r_1 = rgba;
            r_1.r = 1;

            set_gradient_color(r_0, r_1);
            _entry->set_value(rgba.r);
            _scale->set_value(rgba.r);
            goto done;
        }
        else if (_component == 'G')
        {
            auto g_0 = rgba;
            g_0.g = 0;
            auto g_1 = rgba;
            g_1.g = 1;

            set_gradient_color(g_0, g_1);
            _entry->set_value(rgba.g);
            _scale->set_value(rgba.g);
            goto done;
        }
        else if (_component == 'B')
        {
            auto b_0 = rgba;
            b_0.b = 0;
            auto b_1 = rgba;
            b_1.b = 1;

            set_gradient_color(b_0, b_1);
            _entry->set_value(rgba.b);
            _scale->set_value(rgba.b);
            goto done;
        }

        done:
            _entry->set_all_signals_blocked(false);
            _scale->set_all_signals_blocked(false);
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
        _entry->set_hexpand(true);
        _entry->set_margin_start(left_to_slider_left_margin);
    }

    void ColorPicker::HtmlCodeElement::update()
    {
        _entry->set_text(sanitize_html_code(color_to_code(current_color)));
    }

    ColorPicker::HtmlCodeElement::~HtmlCodeElement()
    {
        delete _entry;
    }

    std::string ColorPicker::HtmlCodeElement::sanitize_html_code(const std::string& in)
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

    bool ColorPicker::HtmlCodeElement::is_html_code_valid(const std::string& in)
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

    RGBA ColorPicker::HtmlCodeElement::code_to_color(const std::string& in)
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
            current_color.a
        );

        on_error:
            std::cerr << "[LOG] Unable to parse hex html code \"" << in << "\", returning RGBA(0, 0, 0, 1)" << std::endl;
            return RGBA(0, 0, 0, 1);
    }

    std::string ColorPicker::HtmlCodeElement::color_to_code(RGBA in)
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

        return "#" + r_string + g_string + b_string;
    }

    ColorPicker::EyedropperModeButton::EyedropperModeButton()
    {
        _button = new Button();
        _button->set_label("O,");
        _button->set_tooltip_text("Start Pixel Color Selection");
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
            pair.second->_gradient->set_hexpand(true);
        }

        _opacity_label = new LabelElement("Opacity");
        _hsv_label = new LabelElement("HSV");
        _rgb_label = new LabelElement("RGB");
        _html_label = new LabelElement("HTML Code:");

        _html_code_element = new HtmlCodeElement();
        _html_code_element->set_margin_end(0.6 * width);
        //_html_code_element->_entry->set_width_chars(6 + 2);
        //_html_code_element->set_halign(GTK_ALIGN_START);

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
        _current_color_area->set_valign(GTK_ALIGN_START);
        _current_color_area->set_vexpand(false);
        _current_color_area->set_hexpand(true);
        _current_color_area->set_size_request({1, 4 * margin});
        _current_color_area->set_margin_start(0.6 * width);

        _eyedropper_button = new EyedropperModeButton();
        _eyedropper_button->set_valign(GTK_ALIGN_END);
        _eyedropper_button->set_halign(GTK_ALIGN_END);
        _eyedropper_button->set_size_request({7 * margin, 5 * margin});

        _all_slider_regions->set_margin_top(_current_color_area->get_size_request().y - margin);

        _current_color_and_eyedropper_overlay = new Overlay();
        _current_color_and_eyedropper_overlay->set_under(_all_slider_regions);
        _current_color_and_eyedropper_overlay->set_over(_current_color_area);
        _current_color_and_eyedropper_overlay->set_over(_eyedropper_button);
        _current_color_and_eyedropper_overlay->set_margin_end(outer_margin);
        _current_color_and_eyedropper_overlay->set_margin_start(0.5 * outer_margin);
        _current_color_and_eyedropper_overlay->set_margin_top(0.5 * outer_margin);
        _current_color_and_eyedropper_overlay->set_margin_bottom(0.5 * outer_margin);

        _closing_dialogue = new ClosingDialogue();
        _closing_dialogue->set_halign(GTK_ALIGN_START);
        _closing_dialogue->set_valign(GTK_ALIGN_START);

        _closing_dialogue_overlay = new Overlay();
        _closing_dialogue_overlay->set_under(_current_color_and_eyedropper_overlay);
        _closing_dialogue_overlay->set_over(_closing_dialogue);

        _window = new Box(GTK_ORIENTATION_VERTICAL);
        _closing_dialogue_overlay->set_halign(GTK_ALIGN_CENTER);
        _closing_dialogue_overlay->set_valign(GTK_ALIGN_CENTER);

        _window->add(_closing_dialogue_overlay);

        // tooltips
        _elements.at('A')->_overlay->set_tooltip_text("Transparency / Alpha");
        _elements.at('H')->_overlay->set_tooltip_text("Hue");
        _elements.at('S')->_overlay->set_tooltip_text("Saturation");
        _elements.at('V')->_overlay->set_tooltip_text("Value");
        _elements.at('R')->_overlay->set_tooltip_text("Red");
        _elements.at('G')->_overlay->set_tooltip_text("Green");
        _elements.at('B')->_overlay->set_tooltip_text("Blue");

        _current_color_area->set_tooltip_text("Currently Selected Color");
        _html_code_element->set_tooltip_text("HTML Color Code: #RGB in Hexadecimal");
        _eyedropper_button->set_tooltip_text("Begin Eyedropper Selection");

        connect_signals(this);
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
        delete _current_color_and_eyedropper_overlay;
        delete _closing_dialogue_overlay;
        delete _window;
    }

    void ColorPicker::update_gui(ColorPicker* instance)
    {
        for (auto& pair : instance->_elements)
        {
            pair.second->update();
            pair.second->_gradient->queue_render();
        }

        if (instance->_current_color_area != nullptr)
            instance->_current_color_area->update();

        if (instance->_current_color_area != nullptr)
            instance->_html_code_element->update();
    }

    void ColorPicker::update_color(ColorPicker* instance, char which_component, float value)
    {
        auto as_hsva = current_color;
        auto as_rgba = current_color.operator RGBA();

        switch (which_component)
        {
            case 'A':
                as_hsva.a = value;
                break;
            case 'R':
                as_rgba.r = value;
                as_hsva = as_rgba.operator HSVA();
                break;
            case 'G':
                as_rgba.g = value;
                as_hsva = as_rgba.operator HSVA();
                break;
            case 'B':
                as_rgba.b = value;
                as_hsva = as_rgba.operator HSVA();
                break;
            case 'H':
                as_hsva.h = value;
                break;
            case 'S':
                as_hsva.s = value;
                break;
            case 'V':
                as_hsva.v = value;
                break;
        }

        if (as_hsva.s == 0 and which_component != 'H')
            as_hsva.h = current_color.h;

        if (as_hsva.h == 0 and current_color.h == 1)
            current_color.h = 1;

        if (as_hsva.h == 1 and current_color.h == 0)
            current_color.h = 0;

        current_color = as_hsva;
        current_color_as_rgba = current_color.operator RGBA();
        signal_color_updated();
    }

    void ColorPicker::scale_value_changed(GtkRange* scale, std::pair<ColorPicker*, char>* instance_and_which)
    {
        ColorPicker::update_color(instance_and_which->first, instance_and_which->second, gtk_range_get_value(scale));
        ColorPicker::update_gui(instance_and_which->first);
    }

    void ColorPicker::spin_button_value_changed(GtkSpinButton* button, std::pair<ColorPicker*, char>* instance_and_which)
    {
        ColorPicker::update_color(instance_and_which->first, instance_and_which->second, gtk_spin_button_get_value(button));
        ColorPicker::update_gui(instance_and_which->first);
    }

    void ColorPicker::entry_on_activate(GtkEntry* entry, ColorPicker* instance)
    {
        instance->_html_code_element->_entry->set_all_signals_blocked(true);

        std::string text = HtmlCodeElement::sanitize_html_code(gtk_entry_get_text(entry));
        if (not HtmlCodeElement::is_html_code_valid(text))
        {
            std::cerr << "[LOG] malformatted hexadecimal rgb code, ignoring input." << std::endl;
            return;
        }

        gtk_entry_set_text(entry, text.c_str());
        current_color = HtmlCodeElement::code_to_color(text).operator HSVA();
        update_gui(instance);
        signal_color_updated();

        instance->_html_code_element->_entry->set_all_signals_blocked(false);
    }

    void ColorPicker::entry_on_paste(GtkEntry* entry, ColorPicker* instance)
    {
        gtk_entry_set_text(entry, HtmlCodeElement::sanitize_html_code(gtk_entry_get_text(entry)).c_str());
    }

    gboolean ColorPicker::widget_on_draw(void* , void*, ColorPicker* instance)
    {
        static bool once = true;

        if (once)
        {
            update_gui(instance);
            once = false;
        }

        return FALSE;
    }

    void ColorPicker::connect_signals(ColorPicker* instance)
    {
        for (auto& pair : instance->_elements)
        {
            auto* data = new std::pair<ColorPicker*, char>(instance, pair.first);
            pair.second->_scale->connect_signal("value-changed", scale_value_changed, data);
            pair.second->_entry->connect_signal("value-changed", spin_button_value_changed, data);
        }

        instance->_html_code_element->_entry->connect_signal("activate", entry_on_activate, instance);
        instance->_html_code_element->_entry->connect_signal("paste-clipboard", entry_on_paste, instance);

        instance->_window->connect_signal("draw", widget_on_draw, instance);
    }

    void ColorPicker::update()
    {
        if (not is_initialized())
            return;

        update_gui(this);
    }

    bool ColorPicker::is_initialized() const
    {
        std::cerr << "ColorPicker::is_initialized: TODO" << std::endl;
        return false;
    }
}