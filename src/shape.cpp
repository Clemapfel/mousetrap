//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <include/shader.hpp>
#include <include/gl_common.hpp>
#include <include/shape.hpp>
#include <include/log.hpp>

#include <iostream>
#include <sstream>

namespace mousetrap
{
    Shape::Shape()
    {
        glGenVertexArrays(1, &_vertex_array_id);
        glGenBuffers(1, &_vertex_buffer_id);
    }

    Shape::~Shape()
    {
        if (_vertex_array_id != 0)
            glDeleteVertexArrays(1, &_vertex_array_id);

        if (_vertex_buffer_id != 0)
            glDeleteBuffers(1, &_vertex_buffer_id);
    }

    Shape::Shape(const Shape& other)
    {
        glGenVertexArrays(1, &_vertex_array_id);
        glGenBuffers(1, &_vertex_buffer_id);

        _vertex_data = other._vertex_data;
        _color = other._color;
        _visible = other._visible;
        _render_type = other._render_type;
        _vertices = other._vertices;
        _indices = other._indices;
        _texture = other._texture;

        update_data(true, true, true);
    }

    Shape& Shape::operator=(const Shape& other)
    {
        if (&other == this)
            return *this;

        glGenVertexArrays(1, &_vertex_array_id);
        glGenBuffers(1, &_vertex_buffer_id);

        _vertex_data = other._vertex_data;
        _color = other._color;
        _visible = other._visible;
        _render_type = other._render_type;
        _vertices = other._vertices;
        _indices = other._indices;
        _texture = other._texture;

        update_data();
        return *this;
    }

    Shape::Shape(Shape&& other) noexcept
    {
        _vertex_array_id = other._vertex_array_id;
        _vertex_buffer_id = other._vertex_buffer_id;

        _vertex_data = std::move(other._vertex_data);
        _color = std::move(other._color);
        _visible = std::move(other._visible);
        _render_type = std::move(other._render_type);
        _vertices = std::move(other._vertices);
        _indices = std::move(other._indices);
        _texture = std::move(other._texture);

        other._vertex_buffer_id = 0;
        other._vertex_array_id = 0;

        update_data();
    }

    Shape& Shape::operator=(Shape&& other) noexcept
    {
        _vertex_array_id = other._vertex_array_id;
        _vertex_buffer_id = other._vertex_buffer_id;

        _vertex_data = std::move(other._vertex_data);
        _color = std::move(other._color);
        _visible = std::move(other._visible);
        _render_type = std::move(other._render_type);
        _vertices = std::move(other._vertices);
        _indices = std::move(other._indices);
        _texture = std::move(other._texture);

        other._vertex_buffer_id = 0;
        other._vertex_array_id = 0;

        return *this;
    }

    void Shape::initialize()
    {
        _vertex_data.clear();
        _vertex_data.reserve(_vertices.size());

        for (auto &v: _vertices)
        {
            _vertex_data.emplace_back();
            auto &data = _vertex_data.back();

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
        glBindVertexArray(_vertex_array_id);
        glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer_id);
        glBufferData(GL_ARRAY_BUFFER, _vertex_data.size() * sizeof(VertexInfo), _vertex_data.data(), GL_STATIC_DRAW);

        if (update_position)
        {
            auto position_location = Shader::get_vertex_position_location();
            glEnableVertexAttribArray(position_location);
            glVertexAttribPointer(position_location,
                                  3,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  sizeof(struct VertexInfo),
                                  (GLvoid *) (G_STRUCT_OFFSET(struct VertexInfo, _position))
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
                                  sizeof(struct VertexInfo),
                                  (GLvoid *) (G_STRUCT_OFFSET(struct VertexInfo, _color))
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
                                  sizeof(struct VertexInfo),
                                  (GLvoid *) (G_STRUCT_OFFSET(struct VertexInfo, _texture_coordinates))
            );
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Shape::update_position() const
    {
        for (size_t i = 0; i < _vertices.size(); ++i)
        {
            auto& v = _vertices.at(i);
            auto& data = _vertex_data.at(i);

            auto as_gl_position = to_gl_position(v.position);

            data._position[0] = as_gl_position[0];
            data._position[1] = as_gl_position[1];
            data._position[2] = as_gl_position[2];
        }

        update_data(true, false, false);
    }

    void Shape::update_color() const
    {
        for (size_t i = 0; i < _vertices.size(); ++i)
        {
            auto& v = _vertices.at(i);
            auto& data = _vertex_data.at(i);

            data._color[0] = v.color.r;
            data._color[1] = v.color.g;
            data._color[2] = v.color.b;
            data._color[3] = v.color.a;
        }

        update_data(false, true, false);
    }

    void Shape::update_texture_coordinate() const
    {
        for (size_t i = 0; i < _vertices.size(); ++i)
        {
            auto& v = _vertices.at(i);
            auto& data = _vertex_data.at(i);

            data._texture_coordinates[0] = v.texture_coordinates[0];
            data._texture_coordinates[1] = v.texture_coordinates[1];
        }

        update_data(false, false, true);
    }

    void Shape::render(const Shader& shader, GLTransform transform) const
    {
        if (not _visible)
            return;

        glUseProgram(shader.get_program_id());
        glUniformMatrix4fv(shader.get_uniform_location("_transform"), 1, GL_FALSE, &(transform.transform[0][0]));

        glUniform1i(shader.get_uniform_location("_texture_set"), _texture != nullptr ? GL_TRUE : GL_FALSE);

        if (_texture != nullptr)
            _texture->bind();

        glBindVertexArray(_vertex_array_id);
        glDrawElements(_render_type, _indices.size(), GL_UNSIGNED_INT, _indices.data());

        if (_texture != nullptr)
            _texture->unbind();

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
        _vertices = {};
        _indices = {0};
        _render_type = GL_POINTS;
        initialize();
    }

    void Shape::as_points(const std::vector<Vector2f>& points)
    {
        _vertices.clear();
        _indices.clear();

        for (size_t i = 0; i < points.size(); ++i)
        {
            auto p = points.at(i);
            _vertices.push_back(Vertex(p.x, p.y, _color));
            _indices.push_back(i);
        }

        _render_type = GL_POINTS;
        initialize();
    }

    void Shape::as_triangle(Vector2f a, Vector2f b, Vector2f c)
    {
        _vertices =
        {
        Vertex(a.x, a.y, _color),
        Vertex(b.x, b.y, _color),
        Vertex(c.x, c.y, _color)
        };

        _indices = {0, 1, 2};
        _render_type = GL_TRIANGLES;
        initialize();
    }

    void Shape::as_rectangle(Vector2f top_left, Vector2f size)
    {
        _vertices =
        {
        Vertex(top_left.x, top_left.y, _color),
        Vertex(top_left.x + size.x, top_left.y, _color),
        Vertex(top_left.x + size.x, top_left.y + size.y, _color),
        Vertex(top_left.x, top_left.y + size.y, _color)
        };

        _vertices.at(0).texture_coordinates = {0, 0};
        _vertices.at(1).texture_coordinates = {1, 0};
        _vertices.at(2).texture_coordinates = {1, 1};
        _vertices.at(3).texture_coordinates = {0, 1};

        _indices = {0, 1, 2, 3};
        _render_type = GL_TRIANGLE_FAN;
        initialize();
    }

    void Shape::as_rectangle(Vector2f a, Vector2f b, Vector2f c, Vector2f d)
    {
        _vertices = {
        Vertex(a.x, a.y, _color),
        Vertex(b.x, b.y, _color),
        Vertex(c.x, c.y, _color),
        Vertex(d.x, d.y, _color)
        };

        _vertices.at(0).texture_coordinates = {0, 0};
        _vertices.at(1).texture_coordinates = {1, 0};
        _vertices.at(2).texture_coordinates = {1, 1};
        _vertices.at(3).texture_coordinates = {0, 1};

        _indices = {0, 1, 2, 3};
        _render_type = GL_TRIANGLE_FAN;
        initialize();
    }

    void Shape::as_rectangle_frame(Vector2f top_left, Vector2f outer_size, float x_width, float y_height)
    {
        float x = top_left.x;
        float y = top_left.y;
        float w = outer_size.x;
        float h = outer_size.y;
        float a = x_width;
        float b = y_height;

        auto v = [&](float x, float y) {
            return Vertex(x, y, _color);
        };

        _vertices =
        {
        v(x, y),
        v(x + w, y),
        v(x, y + b),
        v(x + a, y + b),
        v(x + w - a, y + b),
        v(x + w, y + b),
        v(x, y + h - b),
        v(x + a, y + h - b),
        v(x + w - a, y + h - b),
        v(x + w, y + h - b),
        v(x, y + h),
        v(x + w, y + h)
        };

        _indices = {
        0, 1, 5,
        0, 2, 5,
        4, 5, 8,
        5, 8, 9,
        6, 9, 10,
        9, 10, 11,
        2, 3, 7,
        2, 6, 7
        };

        _render_type = GL_TRIANGLES;
        initialize();
    }

    void Shape::as_line(Vector2f a, Vector2f b)
    {
        _vertices =
        {
        Vertex(a.x, a.y, _color),
        Vertex(b.x, b.y, _color)
        };

        _indices = {0, 1};
        _render_type = GL_LINES;
        initialize();
    }

    void Shape::as_lines(const std::vector<std::pair<Vector2f, Vector2f>>& in)
    {
        _vertices.clear();
        for (const auto& pair : in)
        {
            _vertices.emplace_back(pair.first.x, pair.first.y, _color);
            _vertices.emplace_back(pair.second.x, pair.second.y, _color);
        }

        _indices.clear();
        for (size_t i = 0; i < _vertices.size(); ++i)
            _indices.push_back(i);

        _render_type = GL_LINES;
        initialize();
    }

    void Shape::as_circle(Vector2f center, float radius, size_t n_outer_vertices)
    {
        const float step = 360.f / n_outer_vertices;

        _vertices.clear();
        _vertices.push_back(Vertex(center.x, center.y, _color));

        for (float angle = 0; angle < 360; angle += step)
        {
            auto as_radians = angle * M_PI / 180.f;
            _vertices.emplace_back(
            center.x + cos(as_radians) * radius,
            center.y + sin(as_radians) * radius,
            _color
            );
        }

        _indices.clear();
        for (size_t i = 0; i < _vertices.size(); ++i)
            _indices.push_back(i);

        _indices.push_back(1);

        _render_type = GL_TRIANGLE_FAN;
        initialize();
    }

    void Shape::as_ellipse(Vector2f center, float x_radius, float y_radius, size_t n_outer_vertices)
    {
        const float step = 360.f / n_outer_vertices;

        _vertices.clear();
        _vertices.push_back(Vertex(center.x, center.y, _color));

        for (float angle = 0; angle < 360; angle += step)
        {
            auto as_radians = angle * M_PI / 180.f;
            _vertices.emplace_back(
            center.x + cos(as_radians) * x_radius,
            center.y + sin(as_radians) * y_radius,
            _color
            );
        }

        _indices.clear();
        for (size_t i = 0; i < _vertices.size(); ++i)
            _indices.push_back(i);

        _indices.push_back(1);

        _render_type = GL_TRIANGLE_FAN;
        initialize();
    }

    void Shape::as_circular_ring(Vector2f center, float outer_radius, float thickness, size_t n_outer_vertices)
    {
        as_elliptic_ring(center, outer_radius, outer_radius, thickness, thickness, n_outer_vertices);
    }

    void Shape::as_elliptic_ring(Vector2f center, float x_radius, float y_radius, float x_thickness, float y_thickness, size_t n_outer_vertices)
    {
        const float step = 360.f / n_outer_vertices;
        _vertices.clear();

        for (float angle = 0; angle < 360; angle += step)
        {
            auto as_radians = angle * M_PI / 180.f;
            _vertices.emplace_back(
            center.x + cos(as_radians) * x_radius,
            center.y + sin(as_radians) * y_radius,
            _color
            );

            _vertices.emplace_back(
            center.x + cos(as_radians) * (x_radius - x_thickness),
            center.y + sin(as_radians) * (y_radius - y_thickness),
            _color
            );
        }

        _render_type = GL_TRIANGLES;

        _indices.clear();
        for (size_t i = 0; i < n_outer_vertices - 1; ++i)
        {
            auto a = i * 2;
            _indices.push_back(a);
            _indices.push_back(a+2);
            _indices.push_back(a+3);
            _indices.push_back(a);
            _indices.push_back(a+1);
            _indices.push_back(a+3);
        }

        auto a = _vertices.size() - 2;
        _indices.push_back(a);
        _indices.push_back(0);
        _indices.push_back(1);

        _indices.push_back(a);
        _indices.push_back(a+1);
        _indices.push_back(1);

        initialize();
    }

    void Shape::as_line_strip(const std::vector<Vector2f>& positions)
    {
        _vertices.clear();
        _indices.clear();

        size_t i = 0;
        for (auto& position : positions)
        {
            _vertices.emplace_back(position.x, position.y, _color);
            _indices.push_back(i++);
        }

        _render_type = GL_LINE_STRIP;
        initialize();
    }

    void Shape::as_wireframe(const std::vector<Vector2f>& positions_in)
    {
        _vertices.clear();
        _indices.clear();

        auto positions = sort_by_angle(positions_in);

        size_t i = 0;
        for (auto& position : positions)
        {
            _vertices.emplace_back(position.x, position.y, _color);
            _indices.push_back(i++);
        }

        _render_type = GL_LINE_LOOP;
        initialize();
    }

    void Shape::as_wireframe(const Shape& shape)
    {
        _vertices.clear();
        _indices.clear();

        std::vector<Vector2f> vertices;
        for (size_t i = 0; i < shape.get_n_vertices(); ++i)
            vertices.push_back(shape.get_vertex_position(i));

        size_t i = 0;
        for (auto& position : vertices)
        {
            _vertices.emplace_back(position.x, position.y, _color);
            _indices.push_back(i++);
        }

        _render_type = GL_LINE_LOOP;
        initialize();
    }

    void Shape::as_polygon(const std::vector<Vector2f>& positions_in)
    {
        _vertices.clear();
        _indices.clear();

        auto positions = sort_by_angle(positions_in);

        size_t i = 0;
        for (auto& position : positions)
        {
            _vertices.emplace_back(position.x, position.y, _color);
            _indices.push_back(i++);
        }

        _render_type = GL_TRIANGLE_FAN;
        initialize();
    }

    void Shape::set_vertex_color(size_t i, RGBA color)
    {
        if (i > _vertices.size())
        {
            std::stringstream str;
            str << "In mousetrap::Shape::set_vertex_color: index " << i << " out of bounds for an object with " << _vertices.size() << " vertices" <<  std::endl;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            return;
        }

        _vertices.at(i).color = color;
        update_color();
        update_data(false, true, false);
    }

    RGBA Shape::get_vertex_color(size_t index) const
    {
        if (index > _vertices.size())
        {
            std::stringstream str;
            str << "In mousetrap::Shape::get_vertex_color: index " << index << " out of bounds for an object with " << _vertices.size() << " vertices";
            log::critical(str.str(), MOUSETRAP_DOMAIN);

            return RGBA(0, 0, 0, 0);
        }
        return RGBA(_vertices.at(index).color);
    }

    void Shape::set_vertex_position(size_t i, Vector3f position)
    {
        if (i > _vertices.size())
        {
            std::stringstream str;
            str << "[ERROR] In mousetrap::Shape::set_vertex_position: index " << i << " out of bounds for an object with " << _vertices.size() << " vertices";
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            return;
        }

        _vertices.at(i).position = position;
        update_position();
        update_data(true, false, false);
    }

    Vector3f Shape::get_vertex_position(size_t i) const
    {
        if (i > _vertices.size())
        {
            std::stringstream str;
            str << "In mousetrap::Shape::get_vertex_position: index " << i << " out of bounds for an object with " << _vertices.size() << " vertices";
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            return Vector3f();
        }

        return _vertices.at(i).position;
    }


    void Shape::set_vertex_texture_coordinate(size_t i, Vector2f coordinates)
    {
        if (i > _vertices.size())
        {
            std::stringstream str;
            str << "In mousetrap::Shape::set_vertex_texture_coordinate: index " << i << " out of bounds for an object with " << _vertices.size() << " vertices";
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            return;
        }

        _vertices.at(i).texture_coordinates = coordinates;
        update_texture_coordinate();
        update_data(false, false, true);
    }

    Vector2f Shape::get_vertex_texture_coordinate(size_t i) const
    {
        if (i > _vertices.size())
        {
            std::cerr << "[ERROR] In mousetrap::Shape::get_vertex_position: index " << i << " out of bounds for an object with " << _vertices.size() << " vertices" <<  std::endl;
            return Vector2f();
        }

        return _vertices.at(i).texture_coordinates;
    }

    size_t Shape::get_n_vertices() const
    {
        return _vertices.size();
    }

    void Shape::set_color(RGBA color)
    {
        _color = color;

        for (auto& v : _vertices)
            v.color = color;

        update_color();
    }

    void Shape::set_visible(bool b)
    {
        _visible = b;
    }

    bool Shape::get_visible() const
    {
        return _visible;
    }

    Vector2f Shape::get_centroid() const
    {
        static const auto negative_infinity = std::numeric_limits<float>::min();
        static const auto positive_infinity = std::numeric_limits<float>::max();

        Vector3f min = Vector3f(positive_infinity);
        Vector3f max = Vector3f(negative_infinity);

        for (auto& v : _vertices)
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
        for (auto& v : _vertices)
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

        for (auto& v : _vertices)
        {
            min_x = std::min(min_x, v.position.x);
            min_y = std::min(min_y, v.position.y);
            min_z = std::min(min_z, v.position.z);

            max_x = std::max(max_x, v.position.x);
            max_y = std::max(max_y, v.position.y);
            max_z = std::max(max_z, v.position.z);
        }

        return Rectangle{
        {min_x, min_y},
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
        for (auto& v : _vertices)
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

        for (auto& v : _vertices)
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
        return _texture;
    }

    void Shape::set_texture(const TextureObject* texture)
    {
        _texture = texture;
    }
}