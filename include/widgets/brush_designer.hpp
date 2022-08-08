// 
// Copyright 2022 Clemens Cords
// Created on 8/8/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gl_area.hpp>
#include <include/toggle_button.hpp>

namespace mousetrap
{
    class BrushDesigner : public Widget
    {
        public:
            BrushDesigner(float width, size_t n_cols, size_t n_rows);
            ~BrushDesigner();

            GtkWidget* get_native() {
                return _main_over->get_native();
            }

        private:
            static void on_mouse_press(GtkWidget* widget, GdkEventButton* event, BrushDesigner* instance);
            static void on_pointer_motion(GtkWidget *widget, GdkEventMotion *event, BrushDesigner* instance);
            static void on_mouse_release(GtkWidget* widget, GdkEventButton* event, BrushDesigner* instance);
            
            static void on_button_toggled(GtkToggleButton* button, BrushDesigner* instance);
            static void on_scale_value_changed(GtkRange* range, BrushDesigner* instance);
            
            bool _button_active = false;

            enum DrawingMode : bool
            {
                ERASE = false,
                DRAW = true
            };

            static inline const std::string _draw_mode_label =  "DRAW ";
            static inline const std::string _erase_mode_label = "ERASE";

            struct RenderArea : public GLArea
            {
                RenderArea(size_t n_rows, size_t n_cols);
                ~RenderArea();

                void on_realize(GtkGLArea*);
                void on_resize(GtkGLArea*, int, int);

                void switch_shape(Vector2f cursor_position);

                DrawingMode _mode = DRAW;
                float _alpha = 1;

                size_t _w, _h;
                Vector2f _size = {1, 1};

                std::vector<Shape*> _squares;
                std::vector<Shape*> _lines;
                Shape* _background;

                std::set<Shape*> _already_modified; // resets after button release
            };

            RenderArea* _draw_area;

            Scale* _alpha_scale;
            ToggleButton* _draw_erase_toggle;

            Entry* _brush_name;
            Button* _import_button;
            Button* _export_button;

            Box* _name_import_export_hbox;
            Box* _toggle_alpha_hbox;

            Overlay* _main_under; // import export over area
            Overlay* _main_over;  // toggle alpha over (import export over area)
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

        _squares.reserve(_w * _h);

        for (size_t y = 0; y < _h; ++y)
        {
            for (size_t x = 0; x < _w; ++x)
            {
                _squares.push_back(new Shape());
                _squares.back()->as_rectangle({x * size.x, y * size.y}, {size.x, size.y});
                _squares.back()->set_color(RGBA(0, 0, 0, 0));
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
        _background->set_color(RGBA(1, 1, 1, 0.5));
        add_render_task(_background);

        for (auto* s : _squares)
            add_render_task(s);

        for (auto* l : _lines)
        {
            l->set_color(RGBA(0, 0, 0, 1));
            add_render_task(l);
        }

    }

    void BrushDesigner::RenderArea::on_resize(GtkGLArea* area, int w, int h)
    {
        _size = Vector2f(w, h);
        queue_render();
    }

    BrushDesigner::RenderArea::~RenderArea()
    {
        for (auto* s : _squares)
            delete s;

        for (auto* l : _lines)
            delete l;

        delete _background;
    }

    void BrushDesigner::RenderArea::switch_shape(Vector2f cursor_position)
    {
        cursor_position /= _size;

        float x_bounds = 1.f / _w;
        float y_bounds = 1.f / _h;

        for (auto* s : _squares)
        {
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

    void BrushDesigner::on_mouse_press(GtkWidget* widget, GdkEventButton* event, BrushDesigner* instance)
    {
        if (event->button == 1)
        {
            instance->_button_active = true;
            Vector2f pos = {event->x, event->y};
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
            instance->_draw_area->switch_shape(pos);
            instance->_draw_area->queue_render();
        }
    }

    void BrushDesigner::on_button_toggled(GtkToggleButton* button, BrushDesigner* instance)
    {
        if (gtk_toggle_button_get_active(button))
        {
            instance->_draw_area->_mode = ERASE;
            instance->_draw_erase_toggle->set_label(_erase_mode_label);
        }
        else
        {
            instance->_draw_area->_mode = DRAW;
            instance->_draw_erase_toggle->set_label(_draw_mode_label);
        }
    }

    void BrushDesigner::on_scale_value_changed(GtkRange* range, BrushDesigner* instance)
    {
        instance->_draw_area->_alpha = gtk_range_get_value(range);
    }

    BrushDesigner::BrushDesigner(float width, size_t n_cols, size_t n_rows)
    {
        float margin = width / std::max(n_cols, n_rows);
        
        _draw_area = new RenderArea(n_cols, n_rows);

        _draw_area->set_hexpand(true);
        _draw_area->set_vexpand(true);
        _draw_area->set_size_request(Vector2f(std::max(n_cols * margin, n_rows * margin)));

        _draw_area->set_halign(GTK_ALIGN_CENTER);
        _draw_area->set_valign(GTK_ALIGN_CENTER);

        _draw_area->add_events(GDK_BUTTON_PRESS_MASK);
        _draw_area->add_events(GDK_BUTTON_RELEASE_MASK);
        _draw_area->add_events(GDK_POINTER_MOTION_MASK);

        _draw_area->connect_signal("motion-notify-event", on_pointer_motion, this);
        _draw_area->connect_signal("button-press-event", on_mouse_press, this);
        _draw_area->connect_signal("button-release-event", on_mouse_release, this);
        
        _brush_name = new Entry();
        _brush_name->set_text("new_brush");
        
        _import_button = new Button();
        _import_button->set_label("import");
        
        _export_button = new Button();
        _export_button->set_label("export");
        
        _name_import_export_hbox = new Box(GTK_ORIENTATION_HORIZONTAL, margin);
        _name_import_export_hbox->add(_brush_name);
        _name_import_export_hbox->add(_import_button);
        _name_import_export_hbox->add(_export_button);
        _name_import_export_hbox->set_valign(GTK_ALIGN_START);
        
        _draw_erase_toggle = new ToggleButton();

        if (_draw_area->_mode == DRAW)
            _draw_erase_toggle->set_label(_draw_mode_label);
        else
            _draw_erase_toggle->set_label(_erase_mode_label);;

        _draw_erase_toggle->set_tooltip_text("Draw / Erase");
        _draw_erase_toggle->connect_signal("toggled", on_button_toggled, this);
        
        _alpha_scale = new Scale(0, 1, 1.f / 16);
        _alpha_scale->set_value(_draw_area->_alpha);
        _alpha_scale->set_tooltip_text("opacity");
        _alpha_scale->set_draw_value(true);

        _alpha_scale->set_hexpand(true);
        _alpha_scale->connect_signal("value-changed", on_scale_value_changed, this);
        
        _toggle_alpha_hbox = new Box(GTK_ORIENTATION_HORIZONTAL, margin);
        _toggle_alpha_hbox->add(_draw_erase_toggle);
        _toggle_alpha_hbox->add(_alpha_scale);
        _toggle_alpha_hbox->set_valign(GTK_ALIGN_END);
        _toggle_alpha_hbox->set_margin_end(margin);
        
        _main_under = new Overlay();
        _main_under->set_under(_name_import_export_hbox);
        _main_under->set_over(_draw_area);

        _main_over = new Overlay();
        _main_over->set_under(_main_under);
        _main_over->set_over(_toggle_alpha_hbox);

    }

    BrushDesigner::~BrushDesigner()
    {
        delete _draw_area;

        delete _alpha_scale;
        delete _draw_erase_toggle;
    }
}