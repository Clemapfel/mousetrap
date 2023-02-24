// 
// Copyright 2022 Clemens Cords
// Created on 8/17/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

#include <app/tooltip.hpp>
#include <app/project_state.hpp>
#include <app/app_signals.hpp>

namespace mousetrap
{
    class ColorPicker : public AppComponent
    {
        public:
            ColorPicker();
            ~ColorPicker();

            operator Widget*() override;

            void set_color(HSVA);
            HSVA get_color() const;

        private:
            HSVA _color = HSVA(0.5, 0.5, 0.5, 1);

            static void on_render_area_realize(Widget*, ColorPicker* instance);
            static void on_render_area_resize(GLArea*, int, int , ColorPicker* instance);

            bool _hue_bar_active = false;
            void set_hue_bar_cursor(Vector2f);

            bool _hsv_shape_active = false;
            void set_hsv_shape_cursor(Vector2f);

            Vector2f align_to_pixelgrid(Vector2f);

            AspectFrame _main = AspectFrame(1);
            GLArea _render_area;

            Shader* _hue_bar_shader;
            Shape* _hue_bar_shape = nullptr;
            Shape* _hue_bar_frame = nullptr;

            Shape* _hue_bar_cursor = nullptr;
            Shape* _hue_bar_cursor_frame = nullptr;
            Shape* _hue_bar_cursor_window = nullptr;
            Shape* _hue_bar_cursor_window_frame = nullptr;

            Shader* _hsv_shape_shader = nullptr;
            Shape* _hsv_shape = nullptr;
            Shape* _hsv_shape_frame = nullptr;

            Shape* _hsv_shape_cursor = nullptr;
            Shape* _hsv_shape_cursor_frame = nullptr;

            Shape* _hsv_shape_cursor_window = nullptr;
            Shape* _hsv_shape_cursor_window_frame = nullptr;

            Vector2f _canvas_size;
            HSVA _current_color_hsva;
            Vector2f _square_top_left;
            Vector2f _square_size;

            static void on_render_area_motion_enter(MotionEventController* self, gdouble x, gdouble y, ColorPicker* user_data);
            static void on_render_area_motion(MotionEventController* self, gdouble x, gdouble y, ColorPicker* user_data);
            static void on_render_area_button_press(ClickEventController* self, gint n_press, gdouble x, gdouble y, ColorPicker* user_data);
            static void on_render_area_button_release(ClickEventController* self, gint n_press, gdouble x, gdouble y, ColorPicker* user_data);

            ClickEventController _render_area_button_event_controller;
            MotionEventController _render_area_motion_event_controller;

            void update_primary_color(double x, double y);
            void reformat();

            Tooltip _tooltip;
    };
}
