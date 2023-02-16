//
// Created by clem on 2/14/23.
//

#include <app/canvas.hpp>
#include <app/algorithms.hpp>

namespace mousetrap
{
    Canvas::WireframeLayer::WireframeLayer(Canvas* canvas)
            : _owner(canvas)
    {
        _area.connect_signal_realize(on_area_realize, this);
        _area.connect_signal_resize(on_area_resize, this);
        _area.connect_signal_render(on_area_render, this);
    }

    Canvas::WireframeLayer::operator Widget*()
    {
        return &_area;
    }

    void Canvas::WireframeLayer::set_scale(float scale)
    {
        _scale = scale;
        reformat();
        _area.queue_render();
    }

    void Canvas::WireframeLayer::set_offset(Vector2f offset)
    {
        _offset = {offset.x / _canvas_size.x, offset.y / _canvas_size.y};
        reformat();
        _area.queue_render();
    }

    void Canvas::WireframeLayer::set_positions(Vector2i from, Vector2i to)
    {
        _origin_point = from;
        _destination_point = to;

        reformat();
        _area.queue_render();
    }


    void Canvas::WireframeLayer::on_layer_resolution_changed()
    {
        reformat();
        _area.queue_render();
    }

    void Canvas::WireframeLayer::on_area_realize(Widget* widget, WireframeLayer* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        instance->_line_tool_shape = LineToolShape{
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape()
        };

        instance->_line_tool_render_tasks.clear();
        instance->_line_tool_render_tasks.emplace_back(instance->_line_tool_shape.origin_anchor_inner_outline_shape);
        instance->_line_tool_render_tasks.emplace_back(instance->_line_tool_shape.origin_anchor_outer_outline_shape);
        instance->_line_tool_render_tasks.emplace_back(instance->_line_tool_shape.destination_anchor_inner_outline_shape);
        instance->_line_tool_render_tasks.emplace_back(instance->_line_tool_shape.destination_anchor_outer_outline_shape);
        instance->_line_tool_render_tasks.emplace_back(instance->_line_tool_shape.line_outline_shape);

        instance->_line_tool_render_tasks.emplace_back(instance->_line_tool_shape.destination_anchor_shape);
        instance->_line_tool_render_tasks.emplace_back(instance->_line_tool_shape.origin_anchor_shape);
        instance->_line_tool_render_tasks.emplace_back(instance->_line_tool_shape.line_shape);

        instance->reformat();
    }

    void Canvas::WireframeLayer::on_area_resize(GLArea*, int w, int h, WireframeLayer* instance)
    {
        instance->_canvas_size = {w, h};
        instance->reformat();
        instance->_area.queue_render();
    }

    void Canvas::WireframeLayer::reformat()
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

        const RGBA outline_color = RGBA(0, 0, 0, 1);

        Vector2f anchor_radius = {
                std::max<float>(state::margin_unit / _canvas_size.x, 0.5 * pixel_w),
                std::max<float>(state::margin_unit / _canvas_size.y, 0.5 * pixel_h)
        };

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

        std::vector<std::pair<Vector2f, Vector2f>> vertices;

        const size_t vertex_count = 16;
        _line_tool_shape.origin_anchor_shape->as_wireframe(as_ellipse(origin, anchor_radius.x, anchor_radius.y, vertex_count));
        _line_tool_shape.origin_anchor_inner_outline_shape->as_wireframe(as_ellipse(origin, anchor_radius.x - x_eps, anchor_radius.y - y_eps, vertex_count));
        _line_tool_shape.origin_anchor_outer_outline_shape->as_wireframe(as_ellipse(origin, anchor_radius.x + x_eps, anchor_radius.y + y_eps, vertex_count));

        _line_tool_shape.destination_anchor_shape->as_wireframe(as_ellipse(destination, anchor_radius.x, anchor_radius.y, vertex_count));
        _line_tool_shape.destination_anchor_inner_outline_shape->as_wireframe(as_ellipse(destination, anchor_radius.x - x_eps, anchor_radius.y - y_eps, vertex_count));
        _line_tool_shape.destination_anchor_outer_outline_shape->as_wireframe(as_ellipse(destination, anchor_radius.x + x_eps, anchor_radius.y + y_eps, vertex_count));

        _line_tool_shape.line_shape->as_line(origin, destination);

        for (auto* shape : {
            _line_tool_shape.origin_anchor_inner_outline_shape,
            _line_tool_shape.origin_anchor_outer_outline_shape,
            _line_tool_shape.destination_anchor_inner_outline_shape,
            _line_tool_shape.destination_anchor_outer_outline_shape,
            _line_tool_shape.line_outline_shape
        })
            shape->set_color(outline_color);

        auto angle = radians(std::atan2(_destination_point.x - _origin_point.x, _destination_point.y - _origin_point.y)).as_degrees();

        if ((angle >= 360 - 45 and angle <= 360) or (angle >= 0 and angle <= 45) or (angle >= 180 - 45 and angle <= 180 + 45))
            vertices = {
                {origin + Vector2f(-x_eps, 0), destination + Vector2f(-x_eps, 0)},
                {origin + Vector2f(+x_eps, 0), destination + Vector2f(+x_eps, 0)}
            };
        else
            vertices = {
                {origin + Vector2f(0, -y_eps), destination + Vector2f(0, -y_eps)},
                {origin + Vector2f(0, +y_eps), destination + Vector2f(0, +y_eps)}
            };

        _line_tool_shape.line_outline_shape->as_lines(vertices);
    }

    ProjectState::DrawData Canvas::WireframeLayer::draw()
    {
        auto points = generate_line_points(_origin_point, _destination_point);
        auto out = ProjectState::DrawData();
        auto brush = active_state->get_current_brush()->get_image();

        for (auto p : points)
        {
            for (int x = 0; x < brush.get_size().x; ++x)
            {
                for (int y = 0; y < brush.get_size().y; ++y)
                {
                    auto pos = Vector2i(
                            p.x + x - 0.5 * brush.get_size().x,
                            p.y + y - 0.5 * brush.get_size().y
                    );

                    if (brush.get_pixel(x, y).a == 0 or pos.x < 0 or pos.x >= active_state->get_layer_resolution().x or pos.y < 0 or pos.y >= active_state->get_layer_resolution().y)
                        continue;

                    auto color = active_state->get_primary_color();
                    color.a = active_state->get_brush_opacity();
                    out.insert(std::pair<Vector2i, HSVA>(pos, color));
                }
            }
        }

        return out;
    }

    bool Canvas::WireframeLayer::on_area_render(GLArea* area, GdkGLContext*, WireframeLayer* instance)
    {
        area->make_current();

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        for (auto& task : instance->_line_tool_render_tasks)
            task.render();

        glFlush();
        return true;
    }

}