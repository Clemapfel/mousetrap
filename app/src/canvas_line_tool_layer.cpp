//
// Created by clem on 2/14/23.
//

#include <app/canvas.hpp>

namespace mousetrap
{
    Canvas::LineToolLayer::LineToolLayer(Canvas* canvas)
        : _owner(canvas)
    {
        _area.connect_signal_realize(on_area_realize, this);
        _area.connect_signal_resize(on_area_resize, this);
    }

    Canvas::LineToolLayer::operator Widget*()
    {
        return &_area;
    }

    void Canvas::LineToolLayer::set_scale(float scale)
    {
        _scale = scale;
        reformat();
        _area.queue_render();
    }

    void Canvas::LineToolLayer::set_offset(Vector2f offset)
    {
        _offset = {offset.x / _canvas_size.x, offset.y / _canvas_size.y};
        reformat();
        _area.queue_render();
    }

    void Canvas::LineToolLayer::set_line_position(Vector2i from, Vector2i to)
    {
        _origin_point = from;
        _destination_point = to;
        reformat();
        _area.queue_render();
    }

    void Canvas::LineToolLayer::set_line_visible(bool b)
    {
        _visible = b;

        if (not _area.get_is_realized())
            return;

        for (auto* shape : {
            _origin_anchor_shape,
            _origin_anchor_inner_outline_shape,
            _origin_anchor_outer_outline_shape,
            _line_shape,
            _line_outline_shape,
            _destination_anchor_shape,
            _destination_anchor_inner_outline_shape,
            _destination_anchor_outer_outline_shape
        })
            shape->set_visible(_visible);

        _area.queue_render();
    }

    void Canvas::LineToolLayer::on_layer_resolution_changed()
    {
        reformat();
        _area.queue_render();
    }

    void Canvas::LineToolLayer::on_area_realize(Widget* widget, LineToolLayer* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        instance->_origin_anchor_shape = new Shape();
        instance->_origin_anchor_inner_outline_shape = new Shape();
        instance->_origin_anchor_outer_outline_shape = new Shape();
        instance->_origin_anchor_center_shape = new Shape();
        instance->_line_shape = new Shape();
        instance->_line_outline_shape = new Shape();
        instance->_destination_anchor_shape = new Shape();
        instance->_destination_anchor_inner_outline_shape = new Shape();
        instance->_destination_anchor_outer_outline_shape = new Shape();
        instance->_destination_anchor_center_shape = new Shape();

        instance->reformat();

        for (auto* shape : {
                instance->_origin_anchor_shape,
                instance->_line_shape,
                instance->_destination_anchor_shape,
        })
            shape->set_color(RGBA(1, 1, 1, 1));

        for (auto* shape : {
                instance->_origin_anchor_inner_outline_shape,
                instance->_origin_anchor_outer_outline_shape,
                instance->_origin_anchor_center_shape,
                instance->_line_outline_shape,
                instance->_destination_anchor_inner_outline_shape,
                instance->_destination_anchor_outer_outline_shape,
                instance->_destination_anchor_center_shape
        })
            shape->set_color(RGBA(0, 0, 0, 1));

        area->clear_render_tasks();

        area->add_render_task(instance->_origin_anchor_inner_outline_shape);
        area->add_render_task(instance->_origin_anchor_outer_outline_shape);
        area->add_render_task(instance->_origin_anchor_shape);
        area->add_render_task(instance->_destination_anchor_inner_outline_shape);
        area->add_render_task(instance->_destination_anchor_outer_outline_shape);
        area->add_render_task(instance->_destination_anchor_shape);
        area->add_render_task(instance->_line_outline_shape);
        area->add_render_task(instance->_line_shape);
        area->add_render_task(instance->_origin_anchor_center_shape);
        area->add_render_task(instance->_destination_anchor_center_shape);


        area->queue_render();
    }

    void Canvas::LineToolLayer::on_area_resize(GLArea*, int w, int h, LineToolLayer* instance)
    {
        instance->_canvas_size = {w, h};
        instance->reformat();
        instance->_area.queue_render();
    }

    void Canvas::LineToolLayer::reformat()
    {
        if (not _area.get_is_realized())
            return;

        auto layer_resolution = active_state->get_layer_resolution();

        float width = layer_resolution.x / _canvas_size.x;
        float height = layer_resolution.y / _canvas_size.y;

        width *= _scale;
        height *= _scale;

        Vector2f center = {0.5, 0.5};
        center += _offset;

        Vector2f top_left = center - Vector2f{0.5 * width, 0.5 * height};
        float pixel_w = width / layer_resolution.x;
        float pixel_h = height / layer_resolution.y;

        Vector2f origin = {
            top_left.x + pixel_w * _origin_point.x - 0.5 * pixel_w,
            top_left.y + pixel_h * _origin_point.y - 0.5 * pixel_h
        };

        Vector2f destination = {
            top_left.x + pixel_w * _destination_point.x - 0.5 * pixel_w,
            top_left.y + pixel_h * _destination_point.y - 0.5 * pixel_h
        };

        float x_eps = 1.f / _canvas_size.x;
        float y_eps = 1.f / _canvas_size.y;

        Vector2f anchor_radius = {
            std::max<float>(state::margin_unit / _canvas_size.x, 0.5 * pixel_w),
            std::max<float>(state::margin_unit / _canvas_size.y, 0.5 * pixel_h)
        };
        const size_t vertex_count = 16;

        auto as_ellipse = [](Vector2f center, float x_radius, float y_radius, size_t n_vertices){

            const float step = 360.f / n_vertices;

            std::vector<Vector2f> out;

            for (float angle = 0; angle < 360; angle += step)
            {
                auto as_radians = angle * M_PI / 180.f;
                out.emplace_back(
                    center.x + cos(as_radians) * x_radius,
                    center.y + sin(as_radians) * y_radius
                );
            }

            return out;
        };

        _origin_anchor_shape->as_wireframe(as_ellipse(origin, anchor_radius.x, anchor_radius.y, vertex_count));
        _origin_anchor_inner_outline_shape->as_wireframe(as_ellipse(origin, anchor_radius.x - x_eps, anchor_radius.y - y_eps, vertex_count));
        _origin_anchor_outer_outline_shape->as_wireframe(as_ellipse(origin, anchor_radius.x + x_eps, anchor_radius.y + y_eps, vertex_count));

        _destination_anchor_shape->as_wireframe(as_ellipse(destination, anchor_radius.x, anchor_radius.y, vertex_count));
        _destination_anchor_inner_outline_shape->as_wireframe(as_ellipse(destination, anchor_radius.x - x_eps, anchor_radius.y - y_eps, vertex_count));
        _destination_anchor_outer_outline_shape->as_wireframe(as_ellipse(destination, anchor_radius.x + x_eps, anchor_radius.y + y_eps, vertex_count));

        _origin_anchor_center_shape->as_point(origin);
        _origin_anchor_center_shape->as_point(destination);
        _line_shape->as_line(origin, destination);

        auto angle_rad = std::atan2(origin.x - destination.x, origin.y - destination.y);
        float offset_rad = degrees(90).as_radians();

        x_eps *= 1;
        y_eps *= 1;

        Vector2f left_start = {
            origin.x + x_eps * sin(angle_rad - offset_rad),
            origin.y - y_eps * cos(angle_rad - offset_rad)
        };

        Vector2f left_end = {
            destination.x + x_eps * sin(angle_rad - offset_rad),
            destination.y - y_eps * cos(angle_rad - offset_rad)
        };

        Vector2f right_start = {
            origin.x + x_eps * sin(angle_rad + offset_rad),
            origin.y - y_eps * cos(angle_rad + offset_rad)
        };

        Vector2f right_end = {
            destination.x + x_eps * sin(angle_rad + offset_rad),
            destination.y - y_eps * cos(angle_rad + offset_rad)
        };

        _line_outline_shape->as_wireframe({
                                                  {origin.x - x_eps, origin.y - y_eps},
                                                  {origin.x + x_eps, origin.y - y_eps},
                                                  {destination.}
        })
    }


}