//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/gl_common.hpp>
// #if MOUSETRAP_ENABLE_OPENGL_COMPONENT

#include <mousetrap/shader.hpp>
#include <mousetrap/shape.hpp>
#include <mousetrap/log.hpp>

#include <iostream>
#include <sstream>

namespace mousetrap
{
    namespace detail
    {
        DECLARE_NEW_TYPE(ShapeInternal, shape_internal, SHAPE_INTERNAL)

        static void shape_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_SHAPE_INTERNAL(object);
            G_OBJECT_CLASS(shape_internal_parent_class)->finalize(object);

            if (self->vertex_array_id != 0)
                glDeleteVertexArrays(1, &self->vertex_array_id);

            if (self->vertex_buffer_id != 0)
                glDeleteBuffers(1, &self->vertex_buffer_id);

            delete self->color;
            delete self->vertices;
            delete self->indices;
            delete self->vertex_data;
        }

        DEFINE_NEW_TYPE_TRIVIAL_INIT(ShapeInternal, shape_internal, SHAPE_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(ShapeInternal, shape_internal, SHAPE_INTERNAL)

        static ShapeInternal* shape_internal_new()
        {
            auto* self = (ShapeInternal*) g_object_new(shape_internal_get_type(), nullptr);
            shape_internal_init(self);

            glGenVertexArrays(1, &self->vertex_array_id);
            glGenBuffers(1, &self->vertex_buffer_id);

            self->color = new RGBA(1, 1, 1, 1);
            self->is_visible = true;
            self->render_type = GL_TRIANGLE_STRIP;

            self->vertices = new std::vector<Vertex>();
            self->indices = new std::vector<int>();
            self->vertex_data = new std::vector<VertexInfo>();
            self->texture = nullptr;

            return self;
        }
    }
    
    Shape::Shape()
    {
        _internal = detail::shape_internal_new();
        g_object_ref(_internal);
    }

    Shape::~Shape()
    {
        g_object_unref(_internal);
    }

    Shape::Shape(detail::ShapeInternal* internal)
    {
        if (G_IS_OBJECT(_internal))
            g_object_unref(_internal);

        _internal = internal;
        g_object_ref(_internal);
    }

    Shape::Shape(const Shape& other)
    {
        glGenVertexArrays(1, &_internal->vertex_array_id);
        glGenBuffers(1, &_internal->vertex_buffer_id);

        g_object_ref(other._internal);

        _internal->vertex_data = other._internal->vertex_data;
        _internal->color = other._internal->color;
        _internal->is_visible = other._internal->is_visible;
        _internal->render_type = other._internal->render_type;
        _internal->vertices = other._internal->vertices;
        _internal->indices = other._internal->indices;
        _internal->texture = other._internal->texture;

        update_data(true, true, true);
    }

    Shape& Shape::operator=(const Shape& other)
    {
        if (&other == this)
            return *this;

        g_object_ref(other._internal);

        glGenVertexArrays(1, &_internal->vertex_array_id);
        glGenBuffers(1, &_internal->vertex_buffer_id);

        _internal->vertex_data = other._internal->vertex_data;
        _internal->color = other._internal->color;
        _internal->is_visible = other._internal->is_visible;
        _internal->render_type = other._internal->render_type;
        _internal->vertices = other._internal->vertices;
        _internal->indices = other._internal->indices;
        _internal->texture = other._internal->texture;

        update_data();
        return *this;
    }

    Shape::Shape(Shape&& other) noexcept
    {
        g_object_ref(other._internal);

        _internal->vertex_array_id = other._internal->vertex_array_id;
        _internal->vertex_buffer_id = other._internal->vertex_buffer_id;

        _internal->vertex_data = (other._internal->vertex_data);
        _internal->color = (other._internal->color);
        _internal->is_visible = (other._internal->is_visible);
        _internal->render_type = (other._internal->render_type);
        _internal->vertices = (other._internal->vertices);
        _internal->indices = (other._internal->indices);
        _internal->texture = (other._internal->texture);

        other._internal->vertex_buffer_id = 0;
        other._internal->vertex_array_id = 0;

        update_data();
    }

    Shape& Shape::operator=(Shape&& other) noexcept
    {
        g_object_ref(other._internal);

        _internal->vertex_array_id = other._internal->vertex_array_id;
        _internal->vertex_buffer_id = other._internal->vertex_buffer_id;

        _internal->vertex_data = (other._internal->vertex_data);
        _internal->color = (other._internal->color);
        _internal->is_visible = (other._internal->is_visible);
        _internal->render_type = (other._internal->render_type);
        _internal->vertices = (other._internal->vertices);
        _internal->indices = (other._internal->indices);
        _internal->texture = (other._internal->texture);

        other._internal->vertex_buffer_id = 0;
        other._internal->vertex_array_id = 0;

        return *this;
    }

    GLNativeHandle Shape::get_native_handle() const
    {
        return _internal->vertex_array_id;
    }

    NativeObject Shape::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    void Shape::initialize()
    {
        _internal->vertex_data->clear();
        _internal->vertex_data->reserve(_internal->vertices->size());

        for (auto &v : *_internal->vertices)
        {
            _internal->vertex_data->emplace_back();
            auto &data = _internal->vertex_data->back();

            auto as_gl_position = to_gl_position(v.position);

            data._position[0] = as_gl_position[0];
            data._position[1] = as_gl_position[1];
            data._position[2] = as_gl_position[2];

            data._color[0] = v.color.r;
            data._color[1] = v.color.g;
            data._color[2] = v.color.b;
            data._color[3] = v.color.a;

            data._texture_coordinates[0] = v.texture_coordinates[0];
            data._texture_coordinates[1] = v.texture_coordinates[1];
        }

        update_data(true, true, true);
    }

    void Shape::update_data(bool update_position, bool update_color, bool update_tex_coords) const
    {
        glBindVertexArray(_internal->vertex_array_id);
        glBindBuffer(GL_ARRAY_BUFFER, _internal->vertex_buffer_id);
        glBufferData(GL_ARRAY_BUFFER, _internal->vertex_data->size() * sizeof(struct detail::VertexInfo), _internal->vertex_data->data(), GL_STATIC_DRAW);

        if (update_position)
        {
            auto position_location = Shader::get_vertex_position_location();
            glEnableVertexAttribArray(position_location);
            glVertexAttribPointer(position_location,
                                  3,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  sizeof(struct detail::VertexInfo),
                                  (GLvoid *) (G_STRUCT_OFFSET(struct detail::VertexInfo, _position))
            );
        }

        if (update_color)
        {
            auto color_location = Shader::get_vertex_color_location();
            glEnableVertexAttribArray(color_location);
            glVertexAttribPointer(color_location,
                                  4,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  sizeof(struct detail::VertexInfo),
                                  (GLvoid *) (G_STRUCT_OFFSET(struct detail::VertexInfo, _color))
                                  );
        }

        if (update_tex_coords)
        {
            auto texture_coordinate_location = Shader::get_vertex_texture_coordinate_location();
            glEnableVertexAttribArray(texture_coordinate_location);
            glVertexAttribPointer(texture_coordinate_location,
                                  2,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  sizeof(struct detail::VertexInfo),
                                  (GLvoid *) (G_STRUCT_OFFSET(struct detail::VertexInfo, _texture_coordinates))            );
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Shape::update_position() const
    {
        for (size_t i = 0; i < _internal->vertices->size(); ++i)
        {
            auto& v = _internal->vertices->at(i);
            auto& data = _internal->vertex_data->at(i);

            auto as_gl_position = to_gl_position(v.position);

            data._position[0] = as_gl_position[0];
            data._position[1] = as_gl_position[1];
            data._position[2] = as_gl_position[2];
        }

        update_data(true, false, false);
    }

    void Shape::update_color() const
    {
        for (size_t i = 0; i < _internal->vertices->size(); ++i)
        {
            auto& v = _internal->vertices->at(i);
            auto& data = _internal->vertex_data->at(i);

            data._color[0] = v.color.r;
            data._color[1] = v.color.g;
            data._color[2] = v.color.b;
            data._color[3] = v.color.a;
        }

        update_data(false, true, false);
    }

    void Shape::update_texture_coordinate() const
    {
        for (size_t i = 0; i < _internal->vertices->size(); ++i)
        {
            auto& v = _internal->vertices->at(i);
            auto& data = _internal->vertex_data->at(i);

            data._texture_coordinates[0] = v.texture_coordinates[0];
            data._texture_coordinates[1] = v.texture_coordinates[1];
        }

        update_data(false, false, true);
    }

    void Shape::render(const Shader& shader, GLTransform transform) const
    {
        if (not _internal->is_visible)
            return;

        glUseProgram(shader.get_program_id());
        glUniformMatrix4fv(shader.get_uniform_location("_transform"), 1, GL_FALSE, &(transform.transform[0][0]));

        glUniform1i(shader.get_uniform_location("_texture_set"), _internal->texture != nullptr ? GL_TRUE : GL_FALSE);

        if (_internal->texture != nullptr)
            _internal->texture->bind();

        glBindVertexArray(_internal->vertex_array_id);
        glDrawElements(_internal->render_type, _internal->indices->size(), GL_UNSIGNED_INT, _internal->indices->data());

        if (_internal->texture != nullptr)
            _internal->texture->unbind();

        glBindVertexArray(0);
        glUseProgram(0);
    }

    std::vector<Vector2f> Shape::sort_by_angle(const std::vector<Vector2f>& in)
    {
        auto center = Vector2f(0, 0);
        for (const auto& pos : in)
            center += pos;

        size_t n = in.size();
        center /= Vector2f(n, n);

        std::vector<std::pair<Vector2f, Angle>> by_angle;
        for (const auto& pos : in)
            by_angle.emplace_back(pos, radians(std::atan2(pos.x - center.x, pos.y - center.y)));

        std::sort(by_angle.begin(), by_angle.end(), [](const std::pair<Vector2f, Angle>& a, const std::pair<Vector2f, Angle>& b)
        {
            return a.second.as_degrees() < b.second.as_degrees();
        });

        auto out = std::vector<Vector2f>();
        out.reserve(in.size());

        for (auto& pair : by_angle)
            out.push_back(pair.first);

        return out;
    }

    void Shape::as_point(Vector2f a)
    {
        as_points({a});
        _internal->shape_type = detail::ShapeType::POINT;
    }

    void Shape::as_points(const std::vector<Vector2f>& points)
    {
        _internal->vertices->clear();
        _internal->indices->clear();

        for (size_t i = 0; i < points.size(); ++i)
        {
            auto p = points.at(i);
            _internal->vertices->push_back(Vertex(p.x, p.y, *_internal->color));
            _internal->indices->push_back(i);
        }

        _internal->render_type = GL_POINTS;
        _internal->shape_type = detail::ShapeType::POINTS;
        initialize();
    }

    void Shape::as_triangle(Vector2f a, Vector2f b, Vector2f c)
    {
        *_internal->vertices =
        {
            Vertex(a.x, a.y, *_internal->color),
            Vertex(b.x, b.y, *_internal->color),
            Vertex(c.x, c.y, *_internal->color)
        };

        *_internal->indices = {0, 1, 2};
        _internal->render_type = GL_TRIANGLES;
        _internal->shape_type = detail::ShapeType::TRIANGLE;
        initialize();
    }

    void Shape::as_rectangle(Vector2f top_left, Vector2f size)
    {
        *_internal->vertices =
        {
            Vertex(top_left.x, top_left.y, *_internal->color),
            Vertex(top_left.x + size.x, top_left.y, *_internal->color),
            Vertex(top_left.x + size.x, top_left.y + size.y, *_internal->color),
            Vertex(top_left.x, top_left.y + size.y, *_internal->color)
        };

        _internal->vertices->at(0).texture_coordinates = {0, 0};
        _internal->vertices->at(1).texture_coordinates = {1, 0};
        _internal->vertices->at(2).texture_coordinates = {1, 1};
        _internal->vertices->at(3).texture_coordinates = {0, 1};

        *_internal->indices = {0, 1, 2, 3};
        _internal->render_type = GL_TRIANGLE_FAN;
        _internal->shape_type = detail::ShapeType::RECTANGLE;
        initialize();
    }

    void Shape::as_rectangular_frame(Vector2f top_left, Vector2f outer_size, float x_width, float y_height)
    {
        float x = top_left.x;
        float y = top_left.y;
        float w = outer_size.x;
        float h = outer_size.y;
        float a = x_width;
        float b = y_height;

        auto v = [&](float x, float y) {
            return Vertex(x, y, *_internal->color);
        };

        *_internal->vertices =
        {
            v(x, y),
            v(x + w, y),
            v(x, y - b),
            v(x + a, y - b),
            v(x + w - a, y - b),
            v(x + w, y - b),
            v(x, y - h + b),
            v(x + a, y - h + b),
            v(x + w - a, y - h + b),
            v(x + w, y - h + b),
            v(x, y - h),
            v(x + w, y - h)
        };

        *_internal->indices = {
            0, 1, 5,
            0, 5, 2,
            4, 5, 9,
            4, 9, 8,
            6, 9, 11,
            6, 11, 10,
            2, 3, 7,
            2, 7, 6
        };

        _internal->render_type = GL_TRIANGLES;
        _internal->shape_type = detail::ShapeType::RECTANGULAR_FRAME;
        initialize();
    }

    void Shape::as_line(Vector2f a, Vector2f b)
    {
        *_internal->vertices =
        {
            Vertex(a.x, a.y, *_internal->color),
            Vertex(b.x, b.y, *_internal->color)
        };

        *_internal->indices = {0, 1};
        _internal->render_type = GL_LINES;
        _internal->shape_type = detail::ShapeType::LINE;
        initialize();
    }

    void Shape::as_lines(const std::vector<std::pair<Vector2f, Vector2f>>& in)
    {
        _internal->vertices->clear();
        for (const auto& pair : in)
        {
            _internal->vertices->emplace_back(pair.first.x, pair.first.y, *_internal->color);
            _internal->vertices->emplace_back(pair.second.x, pair.second.y, *_internal->color);
        }

        _internal->indices->clear();
        for (size_t i = 0; i < _internal->vertices->size(); ++i)
            _internal->indices->push_back(i);

        _internal->render_type = GL_LINES;
        _internal->shape_type = detail::ShapeType::LINES;
        initialize();
    }

    void Shape::as_circle(Vector2f center, float radius, size_t n_outer_vertices)
    {
        if (n_outer_vertices < 3)
        {
            log::critical("In Shape::as_circle: n_outer_vertices < 3");
            n_outer_vertices = 3;
        }

        as_ellipse(center, radius, radius, n_outer_vertices);
        _internal->shape_type = detail::ShapeType::CIRCLE;
    }

    void Shape::as_ellipse(Vector2f center, float x_radius, float y_radius, size_t n_outer_vertices)
    {
        if (n_outer_vertices < 3)
        {
            log::critical("In Shape::as_ellipse: n_outer_vertices < 3");
            n_outer_vertices = 3;
        }

        const float step = 360.f / n_outer_vertices;

        _internal->vertices->clear();
        _internal->vertices->push_back(Vertex(center.x, center.y, *_internal->color));

        for (float angle = 0; angle < 360; angle += step)
        {
            auto as_radians = angle * M_PI / 180.f;
            _internal->vertices->emplace_back(
                center.x + cos(as_radians) * x_radius,
                center.y + sin(as_radians) * y_radius,
                *_internal->color
            );
        }

        _internal->indices->clear();
        for (size_t i = 0; i < _internal->vertices->size(); ++i)
            _internal->indices->push_back(i);

        _internal->indices->push_back(1);

        _internal->render_type = GL_TRIANGLE_FAN;
        _internal->shape_type = detail::ShapeType::ELLIPSE;
        initialize();
    }

    void Shape::as_circular_ring(Vector2f center, float outer_radius, float thickness, size_t n_outer_vertices)
    {
        as_elliptical_ring(center, outer_radius, outer_radius, thickness, thickness, n_outer_vertices);
        _internal->shape_type = detail::ShapeType::CIRCULAR_RING;
    }

    void Shape::as_elliptical_ring(Vector2f center, float x_radius, float y_radius, float x_thickness, float y_thickness, size_t n_outer_vertices)
    {
        const float step = 360.f / n_outer_vertices;
        _internal->vertices->clear();

        for (float angle = 0; angle < 360; angle += step)
        {
            auto as_radians = angle * M_PI / 180.f;
            _internal->vertices->emplace_back(
                center.x + cos(as_radians) * x_radius,
                center.y + sin(as_radians) * y_radius,
                *_internal->color
            );

            _internal->vertices->emplace_back(
                center.x + cos(as_radians) * (x_radius - x_thickness),
                center.y + sin(as_radians) * (y_radius - y_thickness),
                *_internal->color
            );
        }

        _internal->render_type = GL_TRIANGLES;
        _internal->shape_type = detail::ShapeType::ELLIPTICAL_RING;

        _internal->indices->clear();
        for (size_t i = 0; i < n_outer_vertices - 1; ++i)
        {
            auto a = i * 2;
            _internal->indices->push_back(a);
            _internal->indices->push_back(a+2);
            _internal->indices->push_back(a+3);
            _internal->indices->push_back(a);
            _internal->indices->push_back(a+1);
            _internal->indices->push_back(a+3);
        }

        auto a = _internal->vertices->size() - 2;
        _internal->indices->push_back(a);
        _internal->indices->push_back(0);
        _internal->indices->push_back(1);

        _internal->indices->push_back(a);
        _internal->indices->push_back(a+1);
        _internal->indices->push_back(1);

        initialize();
    }

    void Shape::as_line_strip(const std::vector<Vector2f>& positions)
    {
        _internal->vertices->clear();
        _internal->indices->clear();

        size_t i = 0;
        for (auto& position : positions)
        {
            _internal->vertices->emplace_back(position.x, position.y, *_internal->color);
            _internal->indices->push_back(i++);
        }

        _internal->render_type = GL_LINE_STRIP;
        _internal->shape_type = detail::ShapeType::LINE_STRIP;
        initialize();
    }

    void Shape::as_wireframe(const std::vector<Vector2f>& positions_in)
    {
        _internal->vertices->clear();
        _internal->indices->clear();

        auto positions = sort_by_angle(positions_in);

        size_t i = 0;
        for (auto& position : positions)
        {
            _internal->vertices->emplace_back(position.x, position.y, *_internal->color);
            _internal->indices->push_back(i++);
        }

        _internal->render_type = GL_LINE_LOOP;
        _internal->shape_type = detail::ShapeType::WIREFRAME;
        initialize();
    }

    void Shape::as_polygon(const std::vector<Vector2f>& positions_in)
    {
        _internal->vertices->clear();
        _internal->indices->clear();

        auto positions = sort_by_angle(positions_in);

        size_t i = 0;
        for (auto& position : positions)
        {
            _internal->vertices->emplace_back(position.x, position.y, *_internal->color);
            _internal->indices->push_back(i++);
        }

        _internal->render_type = GL_TRIANGLE_FAN;
        _internal->shape_type = detail::ShapeType::POLYGON;
        initialize();
    }

    void Shape::as_outline(const Shape& shape, RGBA color)
    {
        _internal->vertices->clear();
        _internal->indices->clear();

        std::vector<std::pair<Vector2f, Vector2f>> positions;

        auto type = shape._internal->shape_type;
        using namespace detail;
        if (type == ShapeType::UNKNOWN)
        {
            log::critical("In Shape::as_outline: Attempting to create outline of a shape that is not yet initialized", MOUSETRAP_DOMAIN);
        }
        else if (type == ShapeType::POINT)
        {
            log::warning("In Shape::as_outline: Creating outline of a point, which has an area of 0", MOUSETRAP_DOMAIN);
            positions = {{shape.get_vertex_position(0), shape.get_vertex_position(0)}};
        }
        else if (type == ShapeType::POINTS)
        {
            log::warning("In Shape::as_outline: Creating outline of points, which have an area of 0", MOUSETRAP_DOMAIN);
            for (size_t i = 0; i < shape.get_n_vertices(); ++i)
            {
                positions.push_back({
                    shape.get_vertex_position(i),
                    shape.get_vertex_position(i),
                });
            }
        }
        else if (type == ShapeType::LINE)
        {
            log::warning("In Shape::as_outline: Creating outline of a line, which has an area of 0", MOUSETRAP_DOMAIN);
            positions.push_back({
                shape.get_vertex_position(0),
                shape.get_vertex_position(1)
            });
        }
        else if (type == ShapeType::LINES)
        {
            log::warning("In Shape::as_outline: Creating outline of lines, which have an area of 0", MOUSETRAP_DOMAIN);
            for (size_t i = 0; i < shape.get_n_vertices()-1; i += 2)
            {
                positions.push_back({
                    shape.get_vertex_position(i),
                    shape.get_vertex_position(i + 1)
                });
            }
        }
        else if (type == ShapeType::LINE_STRIP)
        {
            log::warning("In Shape::as_outline: Creating outline of a line strip, which has an area of 0", MOUSETRAP_DOMAIN);
            for (size_t i = 0; i < shape.get_n_vertices()-1; ++i)
            {
                positions.push_back({
                    shape.get_vertex_position(i),
                    shape.get_vertex_position(i + 1)
                });
            }
        }
        else if (type == ShapeType::WIREFRAME)
        {
            log::warning("In Shape::as_outline: Creating outline of a wireframe, which has an area of 0", MOUSETRAP_DOMAIN);
            for (size_t i = 0; i < shape.get_n_vertices()-1; ++i)
            {
                positions.push_back({
                    shape.get_vertex_position(i),
                    shape.get_vertex_position(i + 1)
                });
            }

            positions.push_back({
                shape.get_vertex_position(shape.get_n_vertices()-1),
                shape.get_vertex_position(0)
            });
        }
        else if (type == ShapeType::OUTLINE)
        {
            log::warning("In Shape::as_outline: Creating outline of an outline, which has an area of 0", MOUSETRAP_DOMAIN);
            for (size_t i = 0; i < shape.get_n_vertices()-1; ++i)
            {
                positions.push_back({
                    shape.get_vertex_position(i),
                    shape.get_vertex_position(i + 1)
                });
            }
        }
        else if (type == ShapeType::TRIANGLE)
        {
            positions.push_back({
                shape.get_vertex_position(0),
                shape.get_vertex_position(1)
            });

            positions.push_back({
                shape.get_vertex_position(1),
                shape.get_vertex_position(2)
            });

            positions.push_back({
                shape.get_vertex_position(2),
                shape.get_vertex_position(0)
            });
        }
        else if (type == ShapeType::RECTANGLE)
        {
            auto aabb = shape.get_bounding_box();
            float x = aabb.top_left.x;
            float y = aabb.top_left.y;
            float w = aabb.size.x;
            float h = aabb.size.y;

            positions = {
                {{x, y}, {x + w, y}},
                {{x + w, y}, {x + w, y - h}},
                {{x + w, y - h}, {x, y - h}},
                {{x, y - h}, {x, y}}
            };
        }
        else if (type == ShapeType::CIRCLE or type == ShapeType::ELLIPSE)
        {
            for (size_t i = 1; i < shape.get_n_vertices() - 2; ++i)
                positions.push_back({
                    shape.get_vertex_position(i),
                    shape.get_vertex_position(i+1)
                });

            positions.push_back({
                shape.get_vertex_position(shape.get_n_vertices()-2),
                shape.get_vertex_position(1)
            });
        }
        else if (type == ShapeType::POLYGON)
        {
            for (size_t i = 0; i < shape.get_n_vertices() - 1; ++i)
            {
                positions.push_back({
                shape.get_vertex_position(i),
                shape.get_vertex_position(i + 1)
                });
            }

            positions.push_back({
                shape.get_vertex_position(shape.get_n_vertices()-1),
                shape.get_vertex_position(0)
            });
        }
        else if (type == ShapeType::RECTANGULAR_FRAME)
        {
            // outer

            positions.push_back({
                shape.get_vertex_position(0),
                shape.get_vertex_position(1)
            });

            positions.push_back({
                shape.get_vertex_position(1),
                shape.get_vertex_position(11)
            });

            positions.push_back({
                shape.get_vertex_position(11),
                shape.get_vertex_position(10)
            });

            positions.push_back({
                shape.get_vertex_position(10),
                shape.get_vertex_position(0)
            });

            // inner

            positions.push_back({
                shape.get_vertex_position(3),
                shape.get_vertex_position(4)
            });

            positions.push_back({
                shape.get_vertex_position(4),
                shape.get_vertex_position(8)
            });

            positions.push_back({
                shape.get_vertex_position(8),
                shape.get_vertex_position(7)
            });

            positions.push_back({
                shape.get_vertex_position(7),
                shape.get_vertex_position(3)
            });
        }
        else if (type == ShapeType::CIRCULAR_RING or type == ShapeType::ELLIPTICAL_RING)
        {
            for (size_t i = 0; i < shape.get_n_vertices() - 2; i++)
            {
                positions.push_back({
                    shape.get_vertex_position(i),
                    shape.get_vertex_position(i + 2)
                });
            }

            positions.push_back({
                shape.get_vertex_position(shape.get_n_vertices()-1),
                shape.get_vertex_position(1)
            });

            positions.push_back({
                shape.get_vertex_position(shape.get_n_vertices()-2),
                shape.get_vertex_position(0)
            });
        }

        float hue = 0;
        float hue_step = 1.f / positions.size();

        _internal->vertices->clear();

        for (const auto& pair : positions)
        {
            _internal->vertices->emplace_back(pair.first.x, pair.first.y, *_internal->color);
            _internal->vertices->emplace_back(pair.second.x, pair.second.y, *_internal->color);
        }

        _internal->indices->clear();
        for (size_t i = 0; i < _internal->vertices->size(); ++i)
            _internal->indices->push_back(i);

        _internal->render_type = GL_LINES;
        _internal->shape_type = detail::ShapeType::OUTLINE;
        initialize();
    }

    void Shape::set_vertex_color(size_t i, RGBA color)
    {
        if (i > _internal->vertices->size())
        {
            std::stringstream str;
            str << "In mousetrap::Shape::set_vertex_internal->color: index " << i << " out of bounds for an object with " << _internal->vertices->size() << " vertices" <<  std::endl;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            return;
        }

        _internal->vertices->at(i).color = color;
        update_color();
        update_data(false, true, false);
    }

    RGBA Shape::get_vertex_color(size_t index) const
    {
        if (index > _internal->vertices->size())
        {
            std::stringstream str;
            str << "In mousetrap::Shape::get_vertex_internal->color: index " << index << " out of bounds for an object with " << _internal->vertices->size() << " vertices";
            log::critical(str.str(), MOUSETRAP_DOMAIN);

            return RGBA(0, 0, 0, 0);
        }
        return RGBA(_internal->vertices->at(index).color);
    }

    void Shape::set_vertex_position(size_t i, Vector3f position)
    {
        if (i > _internal->vertices->size())
        {
            std::stringstream str;
            str << "[ERROR] In mousetrap::Shape::set_vertex_position: index " << i << " out of bounds for an object with " << _internal->vertices->size() << " vertices";
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            return;
        }

        _internal->vertices->at(i).position = position;
        update_position();
        update_data(true, false, false);
    }

    Vector3f Shape::get_vertex_position(size_t i) const
    {
        if (i > _internal->vertices->size())
        {
            std::stringstream str;
            str << "In mousetrap::Shape::get_vertex_position: index " << i << " out of bounds for an object with " << _internal->vertices->size() << " vertices";
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            return Vector3f();
        }

        return _internal->vertices->at(i).position;
    }

    void Shape::set_vertex_texture_coordinate(size_t i, Vector2f coordinates)
    {
        if (i > _internal->vertices->size())
        {
            std::stringstream str;
            str << "In mousetrap::Shape::set_vertex_internal->texture_coordinate: index " << i << " out of bounds for an object with " << _internal->vertices->size() << " vertices";
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            return;
        }

        _internal->vertices->at(i).texture_coordinates = coordinates;
        update_texture_coordinate();
        update_data(false, false, true);
    }

    Vector2f Shape::get_vertex_texture_coordinate(size_t i) const
    {
        if (i > _internal->vertices->size())
        {
            std::cerr << "[ERROR] In mousetrap::Shape::get_vertex_position: index " << i << " out of bounds for an object with " << _internal->vertices->size() << " vertices" <<  std::endl;
            return Vector2f();
        }

        return _internal->vertices->at(i).texture_coordinates;
    }

    size_t Shape::get_n_vertices() const
    {
        return _internal->vertices->size();
    }

    void Shape::set_color(RGBA color)
    {
        *_internal->color = color;

        for (auto& v : *_internal->vertices)
            v.color = color;

        update_color();
    }

    void Shape::set_is_visible(bool b)
    {
        _internal->is_visible = b;
    }

    bool Shape::get_is_visible() const
    {
        return _internal->is_visible;
    }

    Vector2f Shape::get_centroid() const
    {
        static const auto negative_infinity = std::numeric_limits<float>::min();
        static const auto positive_infinity = std::numeric_limits<float>::max();

        Vector3f min = Vector3f(positive_infinity);
        Vector3f max = Vector3f(negative_infinity);

        for (auto& v : *_internal->vertices)
        {
            min.x = std::min(min.x, v.position.x);
            min.y = std::min(min.y, v.position.y);
            min.z = std::min(min.z, v.position.z);
            max.x = std::max(max.x, v.position.x);
            max.y = std::max(max.y, v.position.y);
            max.z = std::max(max.z, v.position.z);
        }

        return Vector3f(
        min.x + (max.x - min.x) / 2,
        min.y + (max.y - min.y) / 2,
        min.z + (max.z - min.z) / 2
        );
    }

    void Shape::set_centroid(Vector2f position)
    {
        auto delta = position - get_centroid();
        for (auto& v : *_internal->vertices)
        {
            v.position.x += delta.x;
            v.position.y += delta.y;
        }

        update_position();
        update_data(true, false, false);
    }

    Rectangle Shape::get_bounding_box() const
    {
        float min_x = std::numeric_limits<float>::max();
        float min_y = std::numeric_limits<float>::max();
        float min_z = std::numeric_limits<float>::max();

        float max_x = std::numeric_limits<float>::min();
        float max_y = std::numeric_limits<float>::min();
        float max_z = std::numeric_limits<float>::min();

        for (auto& v : *_internal->vertices)
        {
            min_x = std::min(min_x, v.position.x);
            min_y = std::min(min_y, v.position.y);
            min_z = std::min(min_z, v.position.z);

            max_x = std::max(max_x, v.position.x);
            max_y = std::max(max_y, v.position.y);
            max_z = std::max(max_z, v.position.z);
        }

        return mousetrap::Rectangle{
            {min_x, max_y},
            {max_x - min_x, max_y - min_y}
        };
    }

    Vector2f Shape::get_top_left() const
    {
        return get_bounding_box().top_left;
    }

    Vector2f Shape::get_size() const
    {
        return get_bounding_box().size;
    }

    void Shape::set_top_left(Vector2f position)
    {
        auto delta = position - get_bounding_box().top_left;
        for (auto& v : *_internal->vertices)
        {
            v.position.x += delta.x;
            v.position.y += delta.y;
        }

        update_position();
        update_data(true, false, false);
    }

    void Shape::rotate(Angle angle, Vector2f origin)
    {
        auto transform = GLTransform();
        transform.translate({-1 * origin.x, -1 * origin.y});
        transform.rotate(angle, to_gl_position(get_centroid()));
        transform.translate({origin.x, origin.y});

        for (auto& v : *_internal->vertices)
        {
            auto pos = v.position;
            pos = to_gl_position(pos);
            pos = transform.apply_to(pos);
            pos = from_gl_position(pos);
            v.position = pos;
        }

        update_position();
        update_data(true, false, false);
    }

    const TextureObject* Shape::get_texture() const
    {
        return _internal->texture;
    }

    void Shape::set_texture(const TextureObject* texture)
    {
        _internal->texture = texture;
    }

    Shape::operator GObject*() const
    {
        return G_OBJECT(_internal);
    }
}

namespace mousetrap
{
    Shape Shape::Point(Vector2f position)
    {
        auto out = Shape();
        out.as_point(position);
        return out;
    }

    Shape Shape::Points(const std::vector<Vector2f>& positions)
    {
        auto out = Shape();
        out.as_points(positions);
        return out;
    }

    Shape Shape::Triangle(Vector2f a, Vector2f b, Vector2f c)
    {
        auto out = Shape();
        out.as_triangle(a, b, c);
        return out;
    }

    Shape Shape::Rectangle(Vector2f top_left, Vector2f size)
    {
        auto out = Shape();
        out.as_rectangle(top_left, size);
        return out;
    }

    Shape Shape::Circle(Vector2f center, float radius, size_t n_outer_vertices)
    {
        auto out = Shape();
        out.as_circle(center, radius, n_outer_vertices);
        return out;
    }

    Shape Shape::Ellipse(Vector2f center, float x_radius, float y_radius, size_t n_outer_vertices)
    {
        auto out = Shape();
        out.as_ellipse(center, x_radius, y_radius, n_outer_vertices);
        return out;
    }

    Shape Shape::Line(Vector2f a, Vector2f b)
    {
        auto out = Shape();
        out.as_line(a, b);
        return out;
    }

    Shape Shape::Lines(const std::vector<std::pair<Vector2f, Vector2f>>& points)
    {
        auto out = Shape();
        out.as_lines(points);
        return out;
    }

    Shape Shape::LineStrip(const std::vector<Vector2f>& points)
    {
        auto out = Shape();
        out.as_line_strip(points);
        return out;
    }

    Shape Shape::Polygon(const std::vector<Vector2f>& points)
    {
        auto out = Shape();
        out.as_polygon(points);
        return out;
    }

    Shape Shape::RectangularFrame(Vector2f top_left, Vector2f outer_size, float x_width, float y_width)
    {
        auto out = Shape();
        out.as_rectangular_frame(top_left, outer_size, x_width, y_width);
        return out;
    }

    Shape Shape::CircularRing(Vector2f center, float outer_radius, float thickness, size_t n_outer_vertices)
    {
        auto out = Shape();
        out.as_circular_ring(center, outer_radius, thickness, n_outer_vertices);
        return out;
    }

    Shape Shape::EllipticalRing(Vector2f center, float x_radius, float y_radius, float x_thickness, float y_thickness, size_t n_outer_vertices)
    {
        auto out = Shape();
        out.as_elliptical_ring(center, x_radius, y_radius, x_thickness, y_thickness, n_outer_vertices);
        return out;
    }

    Shape Shape::Wireframe(const std::vector<Vector2f>& points)
    {
        auto out = Shape();
        out.as_wireframe(points);
        return out;
    }

    Shape Shape::Outline(const Shape& shape)
    {
        auto out = Shape();
        out.as_outline(shape);
        return out;
    }
}

// #endif // MOUSETRAP_ENABLE_OPENGL_COMPONENT
