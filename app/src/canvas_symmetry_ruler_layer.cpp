//
// Created by clem on 2/12/23.
//

#include <app/canvas.hpp>

namespace mousetrap
{
    Canvas::SymmetryRulerLayer::SymmetryRulerLayer(Canvas* owner)
        : _owner(owner)
    {
        _area.connect_signal_realize(on_area_realize, this);
        _area.connect_signal_resize(on_area_resize, this);
    }

    Canvas::SymmetryRulerLayer::operator Widget*()
    {
        return &_area;
    }

    void Canvas::SymmetryRulerLayer::on_area_realize(Widget* widget, SymmetryRulerLayer* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        instance->_h_anchor_left= new Shape();
        instance->_h_anchor_left_outline= new Shape();
        instance->_h_anchor_right= new Shape();
        instance->_h_anchor_right_outline= new Shape();
        instance->_h_ruler= new Shape();
        instance->_h_ruler_outline= new Shape();

        instance->_v_anchor_top= new Shape();
        instance->_v_anchor_top_outline= new Shape();
        instance->_v_anchor_bottom= new Shape();
        instance->_v_anchor_bottom_outline= new Shape();
        instance->_v_ruler= new Shape();
        instance->_v_ruler_outline= new Shape();
        
        auto ruler_color = state::settings_file->get_value_as<HSVA>("canvas", "symmetry_ruler_color").operator RGBA();
        
        for (auto* outline : {
            instance->_h_anchor_left_outline,
            instance->_h_anchor_right_outline,
            instance->_v_anchor_top_outline,
            instance->_v_anchor_bottom_outline,
            instance->_h_ruler_outline,
            instance->_v_ruler_outline
        })
            outline->set_color(RGBA(0, 0, 0, ruler_color.a));

        for (auto* ruler : {
            instance->_h_ruler,
            instance->_v_ruler,
            instance->_h_anchor_left,
            instance->_h_anchor_right,
            instance->_v_anchor_top,
            instance->_v_anchor_bottom,
        })
            ruler->set_color(ruler_color);

        instance->on_layer_resolution_changed();
        instance->set_horizontal_symmetry_pixel_position(instance->_h_position);
        instance->set_horizontal_symmetry_active(instance->_h_enabled);
        instance->set_vertical_symmetry_pixel_position(instance->_v_position);
        instance->set_vertical_symmetry_active(instance->_v_enabled);
        
        area->clear_render_tasks();
        
        area->add_render_task(instance->_h_anchor_left);
        area->add_render_task(instance->_h_anchor_right);
        area->add_render_task(instance->_h_anchor_left_outline);
        area->add_render_task(instance->_h_anchor_right_outline);
        area->add_render_task(instance->_h_ruler);
        area->add_render_task(instance->_h_ruler_outline);

        area->add_render_task(instance->_v_anchor_top);
        area->add_render_task(instance->_v_anchor_bottom);
        area->add_render_task(instance->_v_anchor_top_outline);
        area->add_render_task(instance->_v_anchor_bottom_outline);
        area->add_render_task(instance->_v_ruler);
        area->add_render_task(instance->_v_ruler_outline);

        area->queue_render();
    }

    void Canvas::SymmetryRulerLayer::on_area_resize(GLArea*, int w, int h, SymmetryRulerLayer* instance)
    {
        *instance->_canvas_size = {w, h};
        instance->reformat();
        instance->_area.queue_render();
    }

    void Canvas::SymmetryRulerLayer::set_scale(float scale)
    {
        _scale = scale;
        reformat();
        _area.queue_render();
    }

    void Canvas::SymmetryRulerLayer::set_offset(Vector2f offset)
    {
        if (_offset == offset)
            return;

        _offset = {offset.x, offset.y};
        reformat();
        _area.queue_render();
    }

    void Canvas::SymmetryRulerLayer::set_color(HSVA color)
    {
        _color = color;

        if (not _area.get_is_realized())
            return;

        for (auto* shape : {
        _v_anchor_top,
        _v_anchor_bottom,
        _v_ruler,
        _h_anchor_left,
        _h_anchor_right,
        _h_ruler,
        })
            shape->set_color(_color);
    }

    void Canvas::SymmetryRulerLayer::set_horizontal_symmetry_active(bool b)
    {
        _h_enabled = b;
        
        if (not _area.get_is_realized())
            return;
        
        _h_anchor_left->set_visible(_h_enabled);
        _h_anchor_left_outline->set_visible(_h_enabled);
        _h_anchor_right->set_visible(_h_enabled);
        _h_anchor_right_outline->set_visible(_h_enabled);
        _h_ruler->set_visible(_h_enabled);
        _h_ruler_outline->set_visible(_h_enabled);

        _area.queue_render();
    }

    void Canvas::SymmetryRulerLayer::set_vertical_symmetry_active(bool b)
    {
        _v_enabled = b;

        if (not _area.get_is_realized())
            return;

        _v_anchor_top->set_visible(_v_enabled);
        _v_anchor_top_outline->set_visible(_v_enabled);
        _v_anchor_bottom->set_visible(_v_enabled);
        _v_anchor_bottom_outline->set_visible(_v_enabled);
        _v_ruler->set_visible(_v_enabled);
        _v_ruler_outline->set_visible(_v_enabled);

        _area.queue_render();
    }

    void Canvas::SymmetryRulerLayer::set_horizontal_symmetry_pixel_position(size_t px) 
    {
        _h_position = px;
        reformat();
        _area.queue_render();
    }

    void Canvas::SymmetryRulerLayer::set_vertical_symmetry_pixel_position(size_t px)
    {
        _v_position = px;
        reformat();
        _area.queue_render();
    }

    void Canvas::SymmetryRulerLayer::on_layer_resolution_changed() 
    {
        if (_h_position >= active_state->get_layer_resolution().x)
            _h_position = active_state->get_layer_resolution().x - 1;

        if (_h_position >= active_state->get_layer_resolution().y)
            _v_position = active_state->get_layer_resolution().y - 1;

        reformat();
        _area.queue_render();
    }

    void Canvas::SymmetryRulerLayer::reformat()
    {
        if (not _area.get_is_realized())
            return;

        auto layer_resolution = active_state->get_layer_resolution();

        float width = layer_resolution.x / _canvas_size->x;
        float height = layer_resolution.y / _canvas_size->y;

        width *= _scale;
        height *= _scale;

        Vector2f center = {0.5, 0.5};
        center += _offset;

        Vector2f top_left = center - Vector2f{0.5 * width, 0.5 * height};
        float pixel_w = width / layer_resolution.x;
        float pixel_h = height / layer_resolution.y;

        float x_eps = 1 / _canvas_size->x;
        float y_eps = 1 / _canvas_size->y;

        auto align = [&](Vector2f in)
        {
            in.x -= std::fmod(in.x, 1.f / _canvas_size->x);
            in.y -= std::fmod(in.y, 1.f / _canvas_size->y);
            return in;
        };

        Vector2f v = top_left + Vector2f(_v_position * pixel_w + (layer_resolution.x % 2 == 0 ? 0.5 * pixel_w : 0), 0);

        _v_ruler->as_line(
        (v), v + Vector2f(0, height)
        );

        _v_ruler_outline->as_lines({
            {v + Vector2f(-x_eps, 0), v + Vector2f(-x_eps, height)},
            {v + Vector2f(+x_eps, 0), v + Vector2f(+x_eps, height)}
        });
        
        _v_anchor_top->as_polygon({
            v,
            {v.x + pixel_w, v.y - pixel_h},
            {v.x + pixel_w, v.y - 2 * pixel_h},
            {v.x - pixel_w, v.y - 2 * pixel_h},
            {v.x - pixel_w, v.y - pixel_h}
        });
        _v_anchor_top_outline->as_wireframe(*_v_anchor_top);
        
        _v_anchor_bottom->as_polygon({
            {v.x, v.y + height},
            {v.x + pixel_w, v.y + height + pixel_h},
            {v.x + pixel_w, v.y + height + 2 * pixel_h},
            {v.x - pixel_w, v.y + height + 2 * pixel_h},
            {v.x - pixel_w, v.y + height + pixel_h}
        });
        _v_anchor_bottom_outline->as_wireframe(*_v_anchor_bottom);

        Vector2f h = top_left + Vector2f(0, _h_position * pixel_h + (layer_resolution.y % 2 == 0 ? 0.5 * pixel_h : 0));

        _h_ruler->as_line(h, h + Vector2f(width, 0));

        _h_ruler_outline->as_lines({
           {h + Vector2f(0, -y_eps), h + Vector2f(width, -y_eps)},
           {h + Vector2f(0, +y_eps), h + Vector2f(width, +y_eps)},
        });

        _h_anchor_left->as_polygon({
           h,
           {h.x - pixel_w, h.y - pixel_h},
           {h.x - 2 * pixel_w, h.y - pixel_h},
           {h.x - 2 * pixel_w, h.y + pixel_h},
           {h.x - pixel_w, h.y + pixel_h}
        });
        _h_anchor_left_outline->as_wireframe(*_h_anchor_left);

        _h_anchor_right->as_polygon({
            {h.x + width, h.y},
            {h.x + width + pixel_w, h.y - pixel_h},
            {h.x + width + 2*pixel_w, h.y - pixel_h},
            {h.x + width + 2*pixel_w, h.y + pixel_h},
            {h.x + width + pixel_w, h.y + pixel_h}
        });
        _h_anchor_right_outline->as_wireframe(*_h_anchor_right);

        for (auto* shape : {
            _v_anchor_top,
            _v_anchor_bottom,
            _v_ruler,
            _h_anchor_left,
            _h_anchor_right,
            _h_ruler,
        })
            shape->set_color(_color);
    }

    void Canvas::SymmetryRulerLayer::set_cursor_position(Vector2i)
    {
    }
}
