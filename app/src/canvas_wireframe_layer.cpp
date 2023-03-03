//
// Created by clem on 2/14/23.
//

#include <app/canvas.hpp>
#include <app/algorithms.hpp>

namespace mousetrap
{
    Canvas::WireframeLayer::WireframeLayer(Canvas* canvas)
            : _owner(canvas), _render_texture(8)
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

    void Canvas::WireframeLayer::set_widget_cursor_position(Vector2f xy)
    {
        _widget_cursor_position = xy;
        update_highlighting();
    }

    void Canvas::WireframeLayer::set_offset(Vector2f offset)
    {
        if (_offset == offset)
            return;

        _offset = {offset.x, offset.y};
        reformat();
        _area.queue_render();
    }

    void Canvas::WireframeLayer::set_a(Vector2i pixel_pos)
    {
        _a = pixel_pos;
        reformat();
        _area.queue_render();
    }

    void Canvas::WireframeLayer::set_b(Vector2i pixel_pos)
    {
        _b = pixel_pos;
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
        instance->_render_shader = new Shader();
        instance->_render_shader->create_from_file(get_resource_path() + "shaders/msaa_postfx.frag", ShaderType::FRAGMENT);
        instance->_render_texture_shape->as_rectangle({0, 0}, {1, 1});
        instance->_render_texture_shape->set_texture(&instance->_render_texture);
        instance->_render_texture_task = new RenderTask(instance->_render_texture_shape, instance->_render_shader);

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
        instance->_circle_tool_render_tasks.clear();

        for (auto* shape : {
            instance->_rectangle_tool_shape.circle_inner_outline,
            instance->_rectangle_tool_shape.circle_outer_outline,
        })
        {
            instance->_circle_tool_render_tasks.emplace_back(shape);
        }
        
        for (auto* shape :  {
            /*
            instance->_rectangle_tool_shape.top_left_anchor_inner_outline,
            instance->_rectangle_tool_shape.top_left_anchor_outer_outline,
            instance->_rectangle_tool_shape.top_right_anchor_inner_outline,
            instance->_rectangle_tool_shape.top_right_anchor_outer_outline,
            instance->_rectangle_tool_shape.bottom_left_anchor_inner_outline,
            instance->_rectangle_tool_shape.bottom_left_anchor_outer_outline,
            instance->_rectangle_tool_shape.bottom_right_anchor_inner_outline,
            instance->_rectangle_tool_shape.bottom_right_anchor_outer_outline,
            instance->_rectangle_tool_shape.top_anchor_inner_outline,
            instance->_rectangle_tool_shape.top_anchor_outer_outline,
            instance->_rectangle_tool_shape.bottom_anchor_inner_outline,
            instance->_rectangle_tool_shape.bottom_anchor_outer_outline,
            instance->_rectangle_tool_shape.left_anchor_inner_outline,
            instance->_rectangle_tool_shape.left_anchor_outer_outline,
            instance->_rectangle_tool_shape.right_anchor_inner_outline,
            instance->_rectangle_tool_shape.right_anchor_outer_outline,
            instance->_rectangle_tool_shape.center_cross_outline,
             */
            instance->_rectangle_tool_shape.bottom_left_anchor_shape,
            instance->_rectangle_tool_shape.top_right_anchor_shape,
            instance->_rectangle_tool_shape.top_left_anchor_shape,
            instance->_rectangle_tool_shape.bottom_right_anchor_shape,
            instance->_rectangle_tool_shape.top_anchor_shape,
            instance->_rectangle_tool_shape.bottom_anchor_shape,
            instance->_rectangle_tool_shape.left_anchor_shape,
            instance->_rectangle_tool_shape.right_anchor_shape,
            instance->_rectangle_tool_shape.center_cross,
            instance->_rectangle_tool_shape.rectangle_inner_outline,
            instance->_rectangle_tool_shape.rectangle_outer_outline,
            instance->_rectangle_tool_shape.rectangle_shape,
        })
        {
            instance->_rectangle_tool_render_tasks.emplace_back(shape);
            instance->_circle_tool_render_tasks.emplace_back(shape);
        }

        for (auto* shape : {
            instance->_rectangle_tool_shape.circle_inner_outline,
            instance->_rectangle_tool_shape.circle_outer_outline,
            instance->_rectangle_tool_shape.circle
        })
        {
            instance->_circle_tool_render_tasks.emplace_back(shape);
        }

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

        Vector2f top_left_point = {
            std::min(_a.x, _b.x),
            std::min(_a.y, _b.y)
        };

        Vector2f bottom_right_point = {
            std::max(_a.x, _b.x),
            std::max(_a.y, _b.y)
        };

        // lines
        {
            Vector2f origin = {
                top_left.x + pixel_w * top_left_point.x - 0.5 * pixel_w,
                top_left.y + pixel_h * top_left_point.y - 0.5 * pixel_h
            };

            Vector2f destination = {
                    top_left.x + pixel_w * bottom_right_point.x - 0.5 * pixel_w,
                    top_left.y + pixel_h * bottom_right_point.y - 0.5 * pixel_h
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

            auto angle = radians(std::atan2(
                                 top_left_point.x - top_left_point.x,
                                 _b.y - _b.y)
            ).as_degrees();

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
                top_left.x + pixel_w * top_left_point.x - 0.5 * pixel_w,
                top_left.y + pixel_h * top_left_point.y - 0.5 * pixel_h
            };

            Vector2f b = {
                top_left.x + pixel_w * bottom_right_point.x - 0.5 * pixel_w,
                top_left.y + pixel_h * bottom_right_point.y - 0.5 * pixel_h
            };

            float width = b.x - a.x;
            float height = b.y - a.y;

            float x_thickness = x_eps * 10;
            float y_thickness = y_eps * 10;

            _rectangle_tool_shape.rectangle_shape->as_wireframe({
                a,
                a + Vector2f(width, 0),
                a + Vector2f(width, height),
                a + Vector2f(0, height)
            });

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

            _rectangle_tool_shape.circle->as_wireframe(as_ellipse(center, 0.5 * width, 0.5 * height, 64));
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

        update_highlighting();
    }

    void Canvas::WireframeLayer::update_highlighting()
    {
        if (not _area.get_is_realized())
            return;

        auto highlight_color = state::settings_file->get_value_as<HSVA>("canvas", "wireframe_layer_highlight_color");
        auto non_highlight_color = state::settings_file->get_value_as<HSVA>("canvas", "wireframe_layer_non_highlight_color");

        auto cursor_position = Vector2f{
            _widget_cursor_position.x / _canvas_size.x,
            _widget_cursor_position.y / _canvas_size.y
        };

        auto update = [&](Shape* shape){

            auto top_left = shape->get_top_left();
            auto size = shape->get_size();

            if (is_point_in_rectangle(cursor_position, Rectangle{top_left, size}))
                shape->set_color(highlight_color);
            else
                shape->set_color(non_highlight_color);
        };

        for (auto* shape : {
            _line_tool_shape.origin_anchor_shape,
            _line_tool_shape.destination_anchor_shape,
            _rectangle_tool_shape.top_left_anchor_shape,
            _rectangle_tool_shape.top_right_anchor_shape,
            _rectangle_tool_shape.right_anchor_shape,
            _rectangle_tool_shape.bottom_right_anchor_shape,
            _rectangle_tool_shape.bottom_anchor_shape,
            _rectangle_tool_shape.bottom_left_anchor_shape,
            _rectangle_tool_shape.left_anchor_shape,
            _rectangle_tool_shape.top_left_anchor_shape,
            _rectangle_tool_shape.top_anchor_shape,
            _rectangle_tool_shape.center_cross
        })
            update(shape);

        _area.queue_render();
    }

    ProjectState::DrawData Canvas::WireframeLayer::draw()
    {
        auto out = ProjectState::DrawData();

        // TODO
        _render_shader->create_from_file(get_resource_path() + "shaders/msaa_postfx.frag", ShaderType::FRAGMENT);
        _area.queue_render();
        return out;
        // TODO

        if (_line_visible)
        {
            auto points = generate_line_points(_a, _b);
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

        if (_rectangle_visible)
        {
            auto width = abs(_a.x - _b.x);
            auto height = abs(_a.y - _b.y);
            auto top_left = Vector2i(
                std::min(_a.x, _b.x + width),
                std::min(_a.y, _b.y + height)
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

        if (_circle_visible)
        {
            auto width = _a.x - _b.x;
            auto height = _a.y - _b.y;

            auto top_left = Vector2i(
                std::min(_a.x, _b.x + width),
                std::min(_a.y, _b.y + height)
            );

            auto brush = active_state->get_current_brush()->get_image();
            auto points = generate_circle_points(abs(width), abs(height));

            for (auto p : points)
            {
                p += _a;
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

        if (instance->_line_visible)
        {
            for (auto& task: instance->_line_tool_render_tasks)
                task.render();
        }

        if (instance->_circle_visible)
        {
            for (auto& task : instance->_circle_tool_render_tasks)
                task.render();
        }
        else if (instance->_rectangle_visible) // else if sic, render tasks overlap
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

    void Canvas::WireframeLayer::set_line_visible(bool b)
    {
        _line_visible = b;
        _area.queue_render();
    }

    void Canvas::WireframeLayer::set_rectangle_visible(bool b)
    {
        _rectangle_visible = b;
        _area.queue_render();
    }

    void Canvas::WireframeLayer::set_circle_visible(bool b)
    {
        _circle_visible = b;
        _area.queue_render();
    }

    void Canvas::WireframeLayer::update_visibility_from_cursor_pos(Vector2i pixel_pos)
    {
        std::cerr << "[ERROR] In Canvas::WireframeLayer::update_visibility_from_cursor_pos: TODO" << std::endl;
    }
}