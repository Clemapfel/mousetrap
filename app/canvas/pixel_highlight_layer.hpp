// 
// Copyright 2022 Clemens Cords
// Created on 11/4/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Canvas::PixelHighlightLayer::PixelHighlightLayer(Canvas* owner)
            : CanvasLayer(owner)
    {
        _area.connect_signal_realize(on_realize, this);
        _area.connect_signal_resize(on_resize, this);

        _motion_controller.connect_signal_motion(on_motion, this);
    }

    Canvas::PixelHighlightLayer::operator Widget*()
    {
        return &_area;
    }

    Canvas::PixelHighlightLayer::~PixelHighlightLayer()
    {
        delete _outline_shape;
        delete _fill_shape;
        delete _canvas_size;
    }

    void Canvas::PixelHighlightLayer::on_realize(Widget* widget, PixelHighlightLayer* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        instance->_outline_shape = new Shape();
        instance->_fill_shape = new Shape();
        instance->update();
        instance->_area.clear_render_tasks();
        area->add_render_task(instance->_fill_shape);
        area->add_render_task(instance->_outline_shape);
        area->queue_render();
    }

    void Canvas::PixelHighlightLayer::update()
    {
        float width = state::layer_resolution.x / _canvas_size->x;
        float height = state::layer_resolution.y / _canvas_size->y;

        float pixel_w = width / state::layer_resolution.x * _owner->_transform_scale;
        float pixel_h = height / state::layer_resolution.y * _owner->_transform_scale;

        if (_outline_shape == nullptr)
            return;

        _outline_shape->as_wireframe({
                                             {0, 0},
                                             {pixel_w, 0},
                                             {pixel_w, pixel_h},
                                             {0, pixel_h}
                                     });
        _fill_shape->as_rectangle(
                _outline_shape->get_vertex_position(0),
                _outline_shape->get_vertex_position(1),
                _outline_shape->get_vertex_position(2),
                _outline_shape->get_vertex_position(3)
        );

        _outline_shape->set_color(cursor_color);
        _fill_shape->set_color(RGBA(
                cursor_color.r,
                cursor_color.g,
                cursor_color.b,
                0.5 * cursor_color.a
        ));

        _area.queue_render();
    }

    void Canvas::PixelHighlightLayer::on_resize(GLArea* area, int w, int h, PixelHighlightLayer* instance)
    {
        *instance->_canvas_size = {w, h};
        instance->update();
        area->queue_render();
    }

    void Canvas::PixelHighlightLayer::on_motion(MotionEventController*, double x, double y, PixelHighlightLayer* instance)
    {
        float widget_w = instance->_area.get_size().x;
        float widget_h = instance->_area.get_size().y;

        Vector2f pos = {x / widget_w, y / widget_h};

        // align with texture-space pixel grid
        float w = state::layer_resolution.x / instance->_canvas_size->x;
        float h = state::layer_resolution.y / instance->_canvas_size->y;

        Vector2f layer_top_left = {0.5 - w / 2, 0.5 - h / 2};
        layer_top_left = to_gl_position(layer_top_left);
        layer_top_left = instance->_owner->_transform->apply_to(layer_top_left);
        layer_top_left = from_gl_position(layer_top_left);

        Vector2f layer_size = {
                state::layer_resolution.x / instance->_canvas_size->x,
                state::layer_resolution.y / instance->_canvas_size->y
        };

        layer_size *= instance->_owner->_transform_scale;

        float x_dist = (pos.x - layer_top_left.x);
        float y_dist = (pos.y - layer_top_left.y);

        Vector2f pixel_size = {layer_size.x / state::layer_resolution.x, layer_size.y / state::layer_resolution.y};

        x_dist /= pixel_size.x;
        y_dist /= pixel_size.y;

        x_dist = floor(x_dist);
        y_dist = floor(y_dist);

        instance->_owner->set_current_pixel_position(x_dist, y_dist);

        pos.x = layer_top_left.x + x_dist * pixel_size.x;
        pos.y = layer_top_left.y + y_dist * pixel_size.y;

        // align with widget-space pixel grid
        pos *= instance->_area.get_size();
        pos.x = round(pos.x);
        pos.y = round(pos.y);
        pos /= instance->_area.get_size();

        if (instance->_outline_shape != nullptr)
        {
            Vector2f centroid = {pos.x + pixel_size.x * 0.5, pos.y + pixel_size.y * 0.5};
            instance->_fill_shape->set_centroid(centroid);
            instance->_outline_shape->set_centroid(centroid);
        }

        instance->_area.queue_render();
    }
}