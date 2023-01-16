// 
// Copyright 2022 Clemens Cords
// Created on 10/9/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

#include <app/app_component.hpp>
#include <app/global_state.hpp>
#include <app/tooltip.hpp>

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

    namespace state
    {
        static inline VerboseColorPicker* verbose_color_picker = nullptr;
    }
}
