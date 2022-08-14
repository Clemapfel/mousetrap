// 
// Copyright 2022 Clemens Cords
// Created on 8/8/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gl_area.hpp>
#include <include/toggle_button.hpp>
#include <include/frame.hpp>

#include <eigen3/Eigen/Dense>
#include <unordered_set>

namespace mousetrap
{
    class BrushDesigner : public Widget
    {
        public:
            BrushDesigner(float width, size_t n_cols, size_t n_rows);
            ~BrushDesigner();

            GtkWidget* get_native() {
                return _main->get_native();
            }

        private:
            static void on_mouse_press(GtkWidget* widget, GdkEventButton* event, BrushDesigner* instance);
            static void on_pointer_motion(GtkWidget *widget, GdkEventMotion *event, BrushDesigner* instance);
            static void on_mouse_release(GtkWidget* widget, GdkEventButton* event, BrushDesigner* instance);
            
            static void on_draw_erase_toggle(GtkToggleButton* button, BrushDesigner* instance);
            static void on_crosshair_toggle(GtkToggleButton* button, BrushDesigner* instance);
            static void on_clear_clicked(GtkButton* button, BrushDesigner* instance);
            static void on_scale_value_changed(GtkRange* range, BrushDesigner* instance);

            static void on_import(void*, BrushDesigner* instance);
            static void on_export(void*, BrushDesigner* instance);

            bool _button_active = false;
            Vector2f previous_pointer_pos;

            enum DrawingMode : bool
            {
                ERASE = false,
                DRAW = true
            };

            std::string to_string();
            void from_string(const std::string&);

            struct RenderArea : public GLArea
            {
                RenderArea(size_t n_rows, size_t n_cols);
                ~RenderArea();

                void on_realize(GtkGLArea*);
                void on_resize(GtkGLArea*, int, int);

                void switch_shape(Vector2f cursor_position);
                void switch_shape(Line line);

                DrawingMode _mode = DRAW;
                float _alpha = 1;

                size_t _w, _h;
                Vector2f _size = {1, 1};

                Eigen::MatrixX<Shape*> _squares;

                std::vector<Shape*> _lines;
                Shape* _background;
                Shape* _center_line_v;
                Shape* _center_line_h;

                std::set<Shape*> _already_modified; // resets after button release
            };

            RenderArea* _draw_area;
            Frame* _draw_area_aspect_frame;

            Scale* _alpha_scale;
            ToggleButton* _draw_erase_toggle;
            ToggleButton* _show_crosshair_toggle;
            Button* _clear;

            static inline const std::string _draw_toggle_active_tooltip = "draw";
            static inline const std::string _draw_toggle_inactive_tooltip = "erase";

            static inline const std::string _crosshair_toggle_active_tooltip = "hide crosshair";
            static inline const std::string _crosshair_toggle_inactive_tooltip = "show crosshair";

            Entry* _brush_name;
            Button* _import_button;
            Button* _export_button;

            Box* _top_box;
            Box* _bottom_box;

            Box* _main;
    };
}

// ###

namespace mousetrap
{
    BrushDesigner::RenderArea::RenderArea(size_t n_rows, size_t n_cols)
        : _w(n_rows), _h(n_cols)
    {}

    void BrushDesigner::RenderArea::on_realize(GtkGLArea* area)
    {
        gtk_gl_area_make_current(area);

        if (_w == 0 or _h == 0)
            return;

        Vector2f size = {1 / float(_w), 1 / float(_h)};

        _squares.resize(_w, _h);

        for (size_t y = 0; y < _h; ++y)
        {
            for (size_t x = 0; x < _w; ++x)
            {
                auto* shape = new Shape();
                _squares(x, y) = shape;
                shape->as_rectangle({x * size.x, y * size.y}, {size.x, size.y});
                shape->set_color(RGBA(0, 0, 0, 0));
            }
        }

        float eps = 0.001;

        // vertical
        _lines.push_back(new Shape());
        _lines.back()->as_line({0 + eps, 0}, {0 + eps, 1});

        _lines.push_back(new Shape());
        _lines.back()->as_line({1 - eps, 0}, {1 - eps, 1});

        for (size_t i = 0; i < _w; ++i)
        {
            _lines.push_back(new Shape());
            _lines.back()->as_line({i * size.x, 0}, {i * size.x, 1});
        }

        _lines.push_back(new Shape());
        _lines.back()->as_line({0, 0}, {1, 0});

        _lines.push_back(new Shape());
        _lines.back()->as_line({0, 1 - eps}, {1, 1 - eps});

        for (size_t i = 0; i < _h; ++i)
        {
            _lines.push_back(new Shape());
            _lines.back()->as_line({0, i * size.y}, {1, i * size.y});
        }

        _background = new Shape();
        _background->as_rectangle({0, 0}, {1, 1});
        _background->set_color(RGBA(0.5, 0.5, 0.5, 1));
        add_render_task(_background);

        float center_line_value = 0.9;

        _center_line_v = new Shape();
        _center_line_v->as_line({0.5, 0}, {0.5, 1});
        _center_line_v->set_color(RGBA(center_line_value, center_line_value, center_line_value, 1));

        _center_line_h = new Shape();
        _center_line_h->as_line({0, 0.5}, {1, 0.5});
        _center_line_h->set_color(RGBA(center_line_value, center_line_value, center_line_value, 1));

        for (size_t i = 0; i < static_cast<size_t>(_squares.size()); ++i)
            add_render_task(_squares.data()[i]);

        for (auto* l : _lines)
        {
            l->set_color(RGBA(0, 0, 0, 1));
            add_render_task(l);
        }

        add_render_task(_center_line_v);
        add_render_task(_center_line_h);
    }

    void BrushDesigner::RenderArea::on_resize(GtkGLArea* area, int w, int h)
    {
        _size = {w, h};
        queue_render();
    }

    BrushDesigner::RenderArea::~RenderArea()
    {
        for (size_t i = 0; i < _w * _h; ++i)
            delete _squares.data()[i];

        for (auto* l : _lines)
            delete l;

        delete _background;
    }

    void BrushDesigner::RenderArea::switch_shape(Vector2f cursor_position)
    {
        cursor_position /= _size;

        float x_bounds = 1.f / _w;
        float y_bounds = 1.f / _h;

        for (size_t i = 0; i < static_cast<size_t>(_squares.size()); ++i)
        {
            auto* s = _squares.data()[i];
            auto top_left = s->get_vertex_position(0);

            if (_already_modified.find(s) != _already_modified.end())
                continue;

            if (cursor_position.x >= top_left.x and cursor_position.x <= top_left.x + x_bounds and
                cursor_position.y >= top_left.y and cursor_position.y <= top_left.y + y_bounds)
            {
                auto color = s->get_vertex_color(0);
               
                if (_mode == ERASE)
                    color.a = 0;
                else 
                    color.a = _alpha;
                
                s->set_color(color);
                _already_modified.insert(s);
                return;
            }
        }
    }

    void BrushDesigner::RenderArea::switch_shape(Line line)
    {
        line.a /= _size;
        line.b /= _size;

        std::vector<Vector2i> to_switch_pre = {};

        for (int x = 0; x < int(_w); ++x)
        {
            for (int y = 0; y < int(_h); ++y)
            {
                auto* s = _squares(x, y);
                if (_already_modified.find(s) != _already_modified.end())
                    continue;

                if (intersecting(line, Rectangle{s->get_top_left(), s->get_size()}))
                    to_switch_pre.push_back(Vector2i(x, y));
            }
        }

        std::vector<Vector2i> to_switch = to_switch_pre;//smooth_line(to_switch_pre);
        for (auto& pos : to_switch)
        {
            auto* s = _squares(pos.x, pos.y);
            auto color = s->get_vertex_color(0);

            if (_mode == ERASE)
                color.a = 0;
            else
                color.a = _alpha;

            s->set_color(color);
            _already_modified.insert(s);
        }
    }

    std::string BrushDesigner::to_string()
    {
        std::vector<float> values;
        values.reserve(_draw_area->_squares.size());

        for (size_t i = 0; i < static_cast<size_t>(_draw_area->_squares.size()); ++i)
        {
            auto* s = _draw_area->_squares.data()[i];
            values.push_back(s->get_vertex_color(0).a);
        }

        //auto out = Brush(values);
        return ""; // _brush_name->get_text() + " = " + out.to_string();
    }

    void BrushDesigner::from_string(const std::string& string)
    {
        //auto brush = Brush();
        //brush.create_from_string(string);

        std::cerr << "[WARNING] In BrushDesigner::from_string: TODO" << std::endl;
    }

    void BrushDesigner::on_mouse_press(GtkWidget* widget, GdkEventButton* event, BrushDesigner* instance)
    {
        if (event->button == 1)
        {
            instance->_button_active = true;
            Vector2f pos = {event->x, event->y};
            instance->previous_pointer_pos = pos;

            instance->_draw_area->switch_shape(pos);
            instance->_draw_area->queue_render();
        }
    }

    void BrushDesigner::on_mouse_release(GtkWidget* widget, GdkEventButton* event, BrushDesigner* instance)
    {
        if (event->button == 1)
        {
            instance->_button_active = false;

            instance->_draw_area->_already_modified.clear();
            instance->_draw_area->queue_render();
        }
    }

    void BrushDesigner::on_pointer_motion(GtkWidget* widget, GdkEventMotion* event, BrushDesigner* instance)
    {
        if (instance->_button_active)
        {
            Vector2f pos = {event->x, event->y};
            instance->_draw_area->switch_shape(Line{instance->previous_pointer_pos, pos});
            instance->previous_pointer_pos = pos;
            instance->_draw_area->queue_render();
        }
    }

    void BrushDesigner::on_draw_erase_toggle(GtkToggleButton* button, BrushDesigner* instance)
    {
        if (gtk_toggle_button_get_active(button))
        {
            instance->_draw_area->_mode = ERASE;
            instance->_draw_erase_toggle->set_tooltip_text(_draw_toggle_active_tooltip);
        }
        else
        {
            instance->_draw_area->_mode = DRAW;
            instance->_draw_erase_toggle->set_tooltip_text(_draw_toggle_inactive_tooltip);
        }
    }

    void BrushDesigner::on_crosshair_toggle(GtkToggleButton* button, BrushDesigner* instance)
    {
        if (gtk_toggle_button_get_active(button))
        {
            auto color = instance->_draw_area->_center_line_h->get_vertex_color(0);
            color.a = 1;
            instance->_draw_area->_center_line_h->set_color(color);
            instance->_draw_area->_center_line_v->set_color(color);
            instance->_show_crosshair_toggle->set_tooltip_text(_crosshair_toggle_inactive_tooltip);
        }
        else
        {
            auto color = instance->_draw_area->_center_line_h->get_vertex_color(0);
            color.a = 0;
            instance->_draw_area->_center_line_h->set_color(color);
            instance->_draw_area->_center_line_v->set_color(color);
            instance->_show_crosshair_toggle->set_tooltip_text(_crosshair_toggle_inactive_tooltip);
        }
    }

    void BrushDesigner::on_clear_clicked(GtkButton* button, BrushDesigner* instance)
    {
        for (size_t i = 0; i < static_cast<size_t>(instance->_draw_area->_squares.size()); ++i)
            instance->_draw_area->_squares.data()[i]->set_color(RGBA(0, 0, 0, 0));
    }

    void BrushDesigner::on_scale_value_changed(GtkRange* range, BrushDesigner* instance)
    {
        instance->_draw_area->_alpha = gtk_range_get_value(range);
    }

    void BrushDesigner::on_import(void*, BrushDesigner* instance)
    {
        instance->from_string("");
    }

    void BrushDesigner::on_export(void*, BrushDesigner* instance)
    {
        std::cout << instance->to_string() << std::endl;
    }

    BrushDesigner::BrushDesigner(float width, size_t n_cols, size_t n_rows)
    {
        float margin = 0.05 * width;

        _draw_area = new RenderArea(n_cols, n_rows);
        _draw_area->set_expand(true);

        float square_size = std::max<float>(5, 1.f / std::max(n_cols, n_rows));
        _draw_area->set_size_request({square_size * n_cols, square_size* n_rows});

        _draw_area->add_events(GDK_BUTTON_PRESS_MASK);
        _draw_area->add_events(GDK_BUTTON_RELEASE_MASK);
        _draw_area->add_events(GDK_POINTER_MOTION_MASK);

        _draw_area->connect_signal("motion-notify-event", on_pointer_motion, this);
        _draw_area->connect_signal("button-press-event", on_mouse_press, this);
        _draw_area->connect_signal("button-release-event", on_mouse_release, this);

        _draw_area_aspect_frame = new Frame(1);
        _draw_area_aspect_frame->add(_draw_area->get_native());
        _draw_area_aspect_frame->set_shadow_type(GtkShadowType::GTK_SHADOW_NONE);

        _brush_name = new Entry();
        _brush_name->set_text("untitled.brush");
        _brush_name->set_hexpand(true);
        _brush_name->set_halign(GTK_ALIGN_START);
        _brush_name->set_margin_end(margin);

        _import_button = new Button();
        _import_button->set_label("import");
        _import_button->set_tooltip_text("load from file");
        _import_button->connect_signal("clicked", on_import, this);

        _export_button = new Button();
        _export_button->set_label("export");
        _export_button->set_tooltip_text("save to file");
        _export_button->connect_signal("clicked", on_export, this);

        _import_button->set_halign(GTK_ALIGN_END);
        _export_button->set_halign(GTK_ALIGN_END);

        _top_box = new Box(GTK_ORIENTATION_HORIZONTAL, 0);
        _top_box->add(_brush_name);
        _top_box->add(_import_button);
        _top_box->add(_export_button);
        _top_box->set_valign(GTK_ALIGN_START);
        
        _draw_erase_toggle = new ToggleButton();
        _draw_erase_toggle->set_label("D/E");
        _draw_erase_toggle->set_active(false);
        _draw_erase_toggle->set_tooltip_text(_draw_toggle_inactive_tooltip);
        _draw_erase_toggle->connect_signal("toggled", on_draw_erase_toggle, this);

        _show_crosshair_toggle = new ToggleButton();
        _show_crosshair_toggle->set_label("+");
        _show_crosshair_toggle->set_active(true);
        _show_crosshair_toggle->set_tooltip_text(_crosshair_toggle_active_tooltip);
        _show_crosshair_toggle->connect_signal("toggled", on_crosshair_toggle, this);

        _clear = new Button();
        _clear->set_label("C");
        _clear->set_tooltip_text("clear");
        _clear->connect_signal("clicked", on_clear_clicked, this);
        
        _alpha_scale = new Scale(0, 1, 0.1);
        _alpha_scale->set_value(_draw_area->_alpha);
        _alpha_scale->set_tooltip_text("opacity");
        _alpha_scale->set_draw_value(true);

        for (size_t i = 0; i <= 10; ++i)
        {
            _alpha_scale->add_mark(i / 10.f, GTK_POS_LEFT);
            _alpha_scale->add_mark(i / 10.f, GTK_POS_RIGHT);
        }

        _alpha_scale->set_hexpand(true);
        _alpha_scale->set_margin_bottom(margin);
        _alpha_scale->set_valign(GTK_ALIGN_CENTER);

        _alpha_scale->set_hexpand(true);
        _alpha_scale->connect_signal("value-changed", on_scale_value_changed, this);

        _bottom_box = new Box(GTK_ORIENTATION_HORIZONTAL, 0);
        _bottom_box->add(_draw_erase_toggle);
        _bottom_box->set_expand(false);

        _alpha_scale->set_margin_start(margin);
        _alpha_scale->set_margin_end(margin);

        _bottom_box->add(_alpha_scale);
        _bottom_box->add(_show_crosshair_toggle);
        _bottom_box->add(_clear);
        _bottom_box->set_margin_top(margin);

        _top_box->set_valign(GTK_ALIGN_START);
        _bottom_box->set_valign(GTK_ALIGN_END);

        _main = new Box(GTK_ORIENTATION_VERTICAL);
        _main->add(_top_box);
        _main->add(GTK_WIDGET(_draw_area_aspect_frame->get_native()));
        _main->add(_bottom_box);
    }

    BrushDesigner::~BrushDesigner()
    {
        delete _draw_area;

        delete _alpha_scale;
        delete _draw_erase_toggle;
    }
}