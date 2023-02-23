//
// Created by clem on 2/14/23.
//

#include <app/canvas.hpp>
#include <app/algorithms.hpp>

namespace mousetrap
{
    Canvas::WireframeLayer::WireframeLayer(Canvas* canvas)
            : _owner(canvas), _render_texture(16)
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

    void Canvas::WireframeLayer::set_as_line(Vector2i from, Vector2i to)
    {
        _current_mode = LINE;
        _line_start_point = from;
        _line_end_point = to;

        reformat();
        _area.queue_render();
    }

    void Canvas::WireframeLayer::set_as_rectangle(Vector2i a, Vector2i b)
    {
        _current_mode = RECTANGLE;
        _rectangle_a_point = a;
        _rectangle_b_point = b;

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

        instance->_render_texture_shape = new Shape();
        instance->_render_texture_shape->as_rectangle({0, 0}, {1, 1});
        instance->_render_texture_shape->set_texture(&instance->_render_texture);
        instance->_render_texture_task = new RenderTask(instance->_render_texture_shape);

        // lines

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
        instance->_line_tool_render_tasks.emplace_back(instance->_line_tool_shape.origin_anchor_inner_outline);
        instance->_line_tool_render_tasks.emplace_back(instance->_line_tool_shape.origin_anchor_outer_outline);
        instance->_line_tool_render_tasks.emplace_back(instance->_line_tool_shape.destination_anchor_inner_outline);
        instance->_line_tool_render_tasks.emplace_back(instance->_line_tool_shape.destination_anchor_outer_outline);
        instance->_line_tool_render_tasks.emplace_back(instance->_line_tool_shape.line_outline);

        instance->_line_tool_render_tasks.emplace_back(instance->_line_tool_shape.destination_anchor_shape);
        instance->_line_tool_render_tasks.emplace_back(instance->_line_tool_shape.origin_anchor_shape);
        instance->_line_tool_render_tasks.emplace_back(instance->_line_tool_shape.line_shape);

        // rectangle

        instance->_rectangle_tool_shape = RectangleToolShape{
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape()
        };

        instance->_rectangle_tool_render_tasks.clear();
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.top_left_anchor_inner_outline);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.top_left_anchor_outer_outline);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.top_right_anchor_inner_outline);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.top_right_anchor_outer_outline);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.bottom_left_anchor_inner_outline);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.bottom_left_anchor_outer_outline);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.bottom_right_anchor_inner_outline);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.bottom_right_anchor_outer_outline);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.top_anchor_inner_outline);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.top_anchor_outer_outline);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.bottom_anchor_inner_outline);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.bottom_anchor_outer_outline);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.left_anchor_inner_outline);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.left_anchor_outer_outline);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.right_anchor_inner_outline);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.right_anchor_outer_outline);

        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.center_cross_outline);

        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.bottom_left_anchor_shape);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.top_right_anchor_shape);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.top_left_anchor_shape);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.bottom_right_anchor_shape);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.top_anchor_shape);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.bottom_anchor_shape);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.left_anchor_shape);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.right_anchor_shape);

        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.center_cross);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.rectangle_inner_outline);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.rectangle_outer_outline);
        //instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.rectangle_shape);

        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.circle_inner_outline);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.circle_outer_outline);
        instance->_rectangle_tool_render_tasks.emplace_back(instance->_rectangle_tool_shape.circle);

        instance->reformat();
    }

    void Canvas::WireframeLayer::on_area_resize(GLArea* area, int w, int h, WireframeLayer* instance)
    {
        area->make_current();

        instance->_canvas_size = {w, h};
        instance->_render_texture.create(instance->_canvas_size.x, instance->_canvas_size.y);
        instance->reformat();
        instance->_area.queue_render();
    }

    void Canvas::WireframeLayer::reformat()
    {
        if (not _area.get_is_realized())
            return;

        auto layer_resolution = active_state->get_layer_resolution();

        float canvas_width = layer_resolution.x / _canvas_size.x;
        float canvas_height = layer_resolution.y / _canvas_size.y;

        canvas_width *= _scale;
        canvas_height *= _scale;

        Vector2f canvas_center = {0.5, 0.5};
        canvas_center += _offset;

        Vector2f top_left = canvas_center - Vector2f{0.5 * canvas_width, 0.5 * canvas_height};
        float pixel_w = canvas_width / layer_resolution.x;
        float pixel_h = canvas_height / layer_resolution.y;

        float x_eps = 1.f / _canvas_size.x;
        float y_eps = 1.f / _canvas_size.y;

        const RGBA outline_color = RGBA(0.1, 0.1, 0.1, 1);
        const RGBA anchor_color = RGBA(0.9, 0.9, 0.9, 1);

        auto as_ellipse = [](Vector2f center, float x_radius, float y_radius, size_t n_vertices)
        {
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

        Vector2f anchor_radius = {
            std::max<float>(state::margin_unit / _canvas_size.x, 0.5 * pixel_w),
            std::max<float>(state::margin_unit / _canvas_size.y, 0.5 * pixel_h)
        };
        size_t anchor_vertex_count = 4;

        // lines
        {
            Vector2f origin = {
                    top_left.x + pixel_w * _line_start_point.x - 0.5 * pixel_w,
                    top_left.y + pixel_h * _line_start_point.y - 0.5 * pixel_h
            };

            Vector2f destination = {
                    top_left.x + pixel_w * _line_end_point.x - 0.5 * pixel_w,
                    top_left.y + pixel_h * _line_end_point.y - 0.5 * pixel_h
            };
            
            std::vector<std::pair<Vector2f, Vector2f>> vertices;

            _line_tool_shape.origin_anchor_shape->as_wireframe(as_ellipse(origin, anchor_radius.x, anchor_radius.y, anchor_vertex_count));
            _line_tool_shape.origin_anchor_inner_outline->as_wireframe(as_ellipse(origin, anchor_radius.x - x_eps, anchor_radius.y - y_eps, anchor_vertex_count));
            _line_tool_shape.origin_anchor_outer_outline->as_wireframe(as_ellipse(origin, anchor_radius.x + x_eps, anchor_radius.y + y_eps, anchor_vertex_count));

            _line_tool_shape.destination_anchor_shape->as_wireframe(as_ellipse(destination, anchor_radius.x, anchor_radius.y, anchor_vertex_count));
            _line_tool_shape.destination_anchor_inner_outline->as_wireframe(as_ellipse(destination, anchor_radius.x - x_eps, anchor_radius.y - y_eps, anchor_vertex_count));
            _line_tool_shape.destination_anchor_outer_outline->as_wireframe(as_ellipse(destination, anchor_radius.x + x_eps, anchor_radius.y + y_eps, anchor_vertex_count));

            _line_tool_shape.line_shape->as_line(origin, destination);

            for (auto* shape:{
                    _line_tool_shape.origin_anchor_inner_outline,
                    _line_tool_shape.origin_anchor_outer_outline,
                    _line_tool_shape.destination_anchor_inner_outline,
                    _line_tool_shape.destination_anchor_outer_outline,
                    _line_tool_shape.line_outline
            })
                shape->set_color(outline_color);

            for (auto* shape : {
                _line_tool_shape.origin_anchor_shape,
                _line_tool_shape.destination_anchor_shape
            })
                shape->set_color(anchor_color);

            auto angle = radians(std::atan2(_line_end_point.x - _line_start_point.x, _line_end_point.y - _line_start_point.y)).as_degrees();

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

            _line_tool_shape.line_outline->as_lines(vertices);
        }

        // rectangle
        {
            Vector2f a = {
                top_left.x + pixel_w * _rectangle_a_point.x - 0.5 * pixel_w,
                top_left.y + pixel_h * _rectangle_a_point.y - 0.5 * pixel_h
            };

            Vector2f b = {
                top_left.x + pixel_w * _rectangle_b_point.x - 0.5 * pixel_w,
                top_left.y + pixel_h * _rectangle_b_point.y - 0.5 * pixel_h
            };

            float width = b.x - a.x;
            float height = b.y - a.y;

            float x_thickness = x_eps * 10;
            float y_thickness = y_eps * 10;

            _rectangle_tool_shape.rectangle_shape->as_rectangle_frame(
                a,
                {width, height},
                x_thickness,
                y_thickness
            );

            _rectangle_tool_shape.rectangle_outer_outline->as_wireframe({
                a + Vector2f(0 - x_eps, 0 - y_eps),
                a + Vector2f(width + x_eps, 0 - y_eps),
                a + Vector2f(width + x_eps, height + y_eps),
                a + Vector2f(0 - x_eps, height + y_eps)
            });

            _rectangle_tool_shape.rectangle_inner_outline->as_wireframe({
                a + Vector2f(0 + x_eps, 0 + y_eps),
                a + Vector2f(width - x_eps, 0 + y_eps),
                a + Vector2f(width - x_eps, height - y_eps),
                a + Vector2f(0 + x_eps, height - y_eps)
            });

            Vector2f top_left_anchor = a;

            _rectangle_tool_shape.top_left_anchor_shape->as_wireframe(as_ellipse(top_left_anchor, anchor_radius.x, anchor_radius.y, anchor_vertex_count));
            _rectangle_tool_shape.top_left_anchor_inner_outline->as_wireframe(as_ellipse(top_left_anchor, anchor_radius.x - x_eps, anchor_radius.y - y_eps, anchor_vertex_count));
            _rectangle_tool_shape.top_left_anchor_outer_outline->as_wireframe(as_ellipse(top_left_anchor, anchor_radius.x + x_eps, anchor_radius.y + y_eps, anchor_vertex_count));

            _rectangle_tool_shape.top_right_anchor_shape->as_wireframe(as_ellipse(top_left_anchor + Vector2f(width, 0), anchor_radius.x, anchor_radius.y, anchor_vertex_count));
            _rectangle_tool_shape.top_right_anchor_inner_outline->as_wireframe(as_ellipse(top_left_anchor + Vector2f(width, 0), anchor_radius.x - x_eps, anchor_radius.y - y_eps, anchor_vertex_count));
            _rectangle_tool_shape.top_right_anchor_outer_outline->as_wireframe(as_ellipse(top_left_anchor + Vector2f(width, 0), anchor_radius.x + x_eps, anchor_radius.y + y_eps, anchor_vertex_count));

            _rectangle_tool_shape.bottom_left_anchor_shape->as_wireframe(as_ellipse(top_left_anchor + Vector2f(0, height), anchor_radius.x, anchor_radius.y, anchor_vertex_count));
            _rectangle_tool_shape.bottom_left_anchor_inner_outline->as_wireframe(as_ellipse(top_left_anchor + Vector2f(0, height), anchor_radius.x - x_eps, anchor_radius.y - y_eps, anchor_vertex_count));
            _rectangle_tool_shape.bottom_left_anchor_outer_outline->as_wireframe(as_ellipse(top_left_anchor + Vector2f(0, height), anchor_radius.x + x_eps, anchor_radius.y + y_eps, anchor_vertex_count));

            _rectangle_tool_shape.bottom_right_anchor_shape->as_wireframe(as_ellipse(top_left_anchor + Vector2f(width, height), anchor_radius.x, anchor_radius.y, anchor_vertex_count));
            _rectangle_tool_shape.bottom_right_anchor_inner_outline->as_wireframe(as_ellipse(top_left_anchor + Vector2f(width, height), anchor_radius.x - x_eps, anchor_radius.y - y_eps, anchor_vertex_count));
            _rectangle_tool_shape.bottom_right_anchor_outer_outline->as_wireframe(as_ellipse(top_left_anchor + Vector2f(width, height), anchor_radius.x + x_eps, anchor_radius.y + y_eps, anchor_vertex_count));

            _rectangle_tool_shape.top_anchor_shape->as_wireframe(as_ellipse(top_left_anchor + Vector2f(width * 0.5, 0), anchor_radius.x, anchor_radius.y, anchor_vertex_count));
            _rectangle_tool_shape.top_anchor_inner_outline->as_wireframe(as_ellipse(top_left_anchor + Vector2f(width * 0.5, 0), anchor_radius.x - x_eps, anchor_radius.y - y_eps, anchor_vertex_count));
            _rectangle_tool_shape.top_anchor_outer_outline->as_wireframe(as_ellipse(top_left_anchor + Vector2f(width * 0.5, 0), anchor_radius.x + x_eps, anchor_radius.y + y_eps, anchor_vertex_count));

            _rectangle_tool_shape.bottom_anchor_shape->as_wireframe(as_ellipse(top_left_anchor + Vector2f(width * 0.5, height), anchor_radius.x, anchor_radius.y, anchor_vertex_count));
            _rectangle_tool_shape.bottom_anchor_inner_outline->as_wireframe(as_ellipse(top_left_anchor + Vector2f(width * 0.5, height), anchor_radius.x - x_eps, anchor_radius.y - y_eps, anchor_vertex_count));
            _rectangle_tool_shape.bottom_anchor_outer_outline->as_wireframe(as_ellipse(top_left_anchor + Vector2f(width * 0.5, height), anchor_radius.x + x_eps, anchor_radius.y + y_eps, anchor_vertex_count));

            _rectangle_tool_shape.left_anchor_shape->as_wireframe(as_ellipse(top_left_anchor + Vector2f(0, height * 0.5), anchor_radius.x, anchor_radius.y, anchor_vertex_count));
            _rectangle_tool_shape.left_anchor_inner_outline->as_wireframe(as_ellipse(top_left_anchor + Vector2f(0, height * 0.5), anchor_radius.x - x_eps, anchor_radius.y - y_eps, anchor_vertex_count));
            _rectangle_tool_shape.left_anchor_outer_outline->as_wireframe(as_ellipse(top_left_anchor + Vector2f(0, height * 0.5), anchor_radius.x + x_eps, anchor_radius.y + y_eps, anchor_vertex_count));

            _rectangle_tool_shape.right_anchor_shape->as_wireframe(as_ellipse(top_left_anchor + Vector2f(width, height * 0.5), anchor_radius.x, anchor_radius.y, anchor_vertex_count));
            _rectangle_tool_shape.right_anchor_inner_outline->as_wireframe(as_ellipse(top_left_anchor + Vector2f(width, height * 0.5), anchor_radius.x - x_eps, anchor_radius.y - y_eps, anchor_vertex_count));
            _rectangle_tool_shape.right_anchor_outer_outline->as_wireframe(as_ellipse(top_left_anchor + Vector2f(width, height * 0.5), anchor_radius.x + x_eps, anchor_radius.y + y_eps, anchor_vertex_count));

            auto center = top_left_anchor + Vector2f(0.5 * width, 0.5 * height);

            _rectangle_tool_shape.circle->as_elliptic_ring(
                center,
                0.5 * width,
                0.5 * height,
                x_thickness,
                y_thickness,
                64
            );

            _rectangle_tool_shape.circle_inner_outline->as_wireframe(as_ellipse(center, 0.5 * width - x_eps, 0.5 * height - y_eps, 64));
            _rectangle_tool_shape.circle_outer_outline->as_wireframe(as_ellipse(center, 0.5 * width + x_eps, 0.5 * height + y_eps, 64));

            float cross_w = anchor_radius.x;
            float cross_h = anchor_radius.y;

            _rectangle_tool_shape.center_cross->as_lines({
                {center + Vector2f(-cross_w, 0), center + Vector2f(+cross_w, 0)},
                {center + Vector2f(0, -cross_h), center + Vector2f(0, +cross_h)}
            });

            _rectangle_tool_shape.center_cross_outline->as_lines({
                {center + Vector2f(-(cross_w + x_eps), 0), center + Vector2f(+(cross_w + x_eps), 0)},
                {center + Vector2f(-cross_w, y_eps), center + Vector2f(+cross_w, y_eps)},
                {center + Vector2f(-cross_w, -y_eps), center + Vector2f(+cross_w, -y_eps)},
                {center + Vector2f(0, -(cross_h + y_eps)), center + Vector2f(0, +(cross_h + y_eps))},
                {center + Vector2f(+x_eps, -cross_h), center + Vector2f(+x_eps, +cross_h)},
                {center + Vector2f(-x_eps, -cross_h), center + Vector2f(-x_eps, +cross_h)}
            });

            for (auto* shape : {
                _rectangle_tool_shape.rectangle_outer_outline,
                _rectangle_tool_shape.rectangle_inner_outline,
                _rectangle_tool_shape.top_left_anchor_inner_outline,
                _rectangle_tool_shape.top_left_anchor_outer_outline,
                _rectangle_tool_shape.top_right_anchor_inner_outline,
                _rectangle_tool_shape.top_right_anchor_outer_outline,
                _rectangle_tool_shape.bottom_left_anchor_inner_outline,
                _rectangle_tool_shape.bottom_left_anchor_outer_outline,
                _rectangle_tool_shape.bottom_right_anchor_inner_outline,
                _rectangle_tool_shape.bottom_right_anchor_outer_outline,
                _rectangle_tool_shape.top_anchor_outer_outline,
                _rectangle_tool_shape.bottom_anchor_outer_outline,
                _rectangle_tool_shape.left_anchor_outer_outline,
                _rectangle_tool_shape.right_anchor_outer_outline,
                _rectangle_tool_shape.top_anchor_inner_outline,
                _rectangle_tool_shape.bottom_anchor_inner_outline,
                _rectangle_tool_shape.left_anchor_inner_outline,
                _rectangle_tool_shape.right_anchor_inner_outline,
                _rectangle_tool_shape.center_cross_outline,
                _rectangle_tool_shape.circle_inner_outline,
                _rectangle_tool_shape.circle_outer_outline
            })
                shape->set_color(outline_color);

            for (auto* shape : {
                _rectangle_tool_shape.top_left_anchor_shape,
                _rectangle_tool_shape.top_right_anchor_shape,
                _rectangle_tool_shape.bottom_left_anchor_shape,
                _rectangle_tool_shape.bottom_right_anchor_shape,
                _rectangle_tool_shape.top_anchor_shape,
                _rectangle_tool_shape.bottom_anchor_shape,
                _rectangle_tool_shape.left_anchor_shape,
                _rectangle_tool_shape.right_anchor_shape,
                _rectangle_tool_shape.center_cross
            })
                shape->set_color(anchor_color);
        }
    }

    ProjectState::DrawData Canvas::WireframeLayer::draw()
    {
        auto out = ProjectState::DrawData();

        if (_current_mode == LINE)
        {
            auto points = generate_line_points(_line_start_point, _line_end_point);
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
        }
        else if (_current_mode == RECTANGLE)
        {
            auto width = _rectangle_b_point.x - _rectangle_a_point.x;
            auto height =_rectangle_b_point.y - _rectangle_a_point.y;
            auto top_left = Vector2i(
                std::min(_rectangle_a_point.x, _rectangle_a_point.x + width),
                std::min(_rectangle_a_point.y, _rectangle_a_point.y + height)
            );

            auto brush = active_state->get_current_brush()->get_image();
            auto points = generate_rectangle_points(top_left, abs(width), abs(height));

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
        }

        return out;
    }

    bool Canvas::WireframeLayer::on_area_render(GLArea* area, GdkGLContext*, WireframeLayer* instance)
    {
        area->make_current();

        instance->_render_texture.bind_as_rendertarget();

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        if (instance->_current_mode == LINE)
        {
            for (auto& task: instance->_line_tool_render_tasks)
                task.render();
        }
        else if (instance->_current_mode == RECTANGLE)
        {
            for (auto& task : instance->_rectangle_tool_render_tasks)
                task.render();
        }

        instance->_render_texture.unbind_as_rendertarget();

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        instance->_render_texture_task->render();

        glFlush();
        return true;
    }

}