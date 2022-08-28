// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <include/shader.hpp>
#include <include/gl_common.hpp>

namespace mousetrap
{
    Shape::Shape()
    {
        glGenVertexArrays(1, &_vertex_array_id);
        glGenBuffers(1, &_vertex_buffer_id);
    }

    Shape::~Shape()
    {
        glDeleteVertexArrays(1, &_vertex_array_id);
        glDeleteBuffers(1, &_vertex_buffer_id);
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

    void Shape::update_data(bool update_position, bool update_color, bool update_tex_coords)
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
        glBindVertexArray(0);
    }

    void Shape::update_position()
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

    void Shape::update_color()
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

    void Shape::update_texture_coordinate()
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

    void Shape::render(Shader& shader, GLTransform transform)
    {
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

    void Shape::as_line_strip(std::vector<Vector2f> positions)
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

    void Shape::as_wireframe(std::vector<Vector2f> positions)
    {
        _vertices.clear();
        _indices.clear();

        positions = sort_by_angle(positions);

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

    void Shape::as_polygon(std::vector<Vector2f> positions)
    {
        _vertices.clear();
        _indices.clear();

        positions = sort_by_angle(positions);

        size_t i = 0;
        for (auto& position : positions)
        {
            _vertices.emplace_back(position.x, position.y, _color);
            _indices.push_back(i++);
        }

        _render_type = GL_TRIANGLE_FAN;
        initialize();
    }

    void Shape::as_frame(Vector2f top_left, Vector2f size, float x_width, float y_width)
    {
        _vertices.clear();
        _indices.clear();

        // hard-coded minimum vertex decomposition

        auto push_vertex = [&](float x, float y) {
            _vertices.emplace_back(x, y, _color);
        };

        float x = top_left.x;
        float y = top_left.y;
        float w = size.x;
        float h = size.y;
        float lx = x_width;
        float ly = y_width;

        // in order: left to right, top to bottom

        push_vertex(x, y);         // 0
        push_vertex(x+w-lx, y);     // 1
        push_vertex(x+w, y);       // 2

        push_vertex(x, y+ly);       // 3
        push_vertex(x+lx, y+ly);     // 4
        push_vertex(x+w-lx, y+ly);   // 5

        push_vertex(x+lx, y+h-ly);   // 6
        push_vertex(x+w-lx, y+h-ly); // 7
        push_vertex(x+w, y+h-ly);   // 8

        push_vertex(x, y+h);       // 9
        push_vertex(x+lx, y+h);     // 10
        push_vertex(x+w, y+h);     // 11

        _indices = {
                0, 1, 5, 0, 5, 3,
                1, 2, 7, 2, 7, 8,
                6, 11, 10, 6, 8, 11,
                3, 9, 10, 3, 4, 10
        };

        _render_type = GL_TRIANGLES;
        initialize();
    }

    void Shape::set_vertex_color(size_t i, RGBA color)
    {
        _vertices.at(i).color = color;
        update_color();
        update_data(false, true, false);
    }

    RGBA Shape::get_vertex_color(size_t index) const
    {
        return RGBA(_vertices.at(index).color);
    }

    void Shape::set_vertex_position(size_t i, Vector3f position)
    {
        _vertices.at(i).position = position;
        update_position();
        update_data(true, false, false);
    }

    Vector3f Shape::get_vertex_position(size_t i) const
    {
        return _vertices.at(i).position;
    }


    void Shape::set_vertex_texture_coordinate(size_t i, Vector2f coordinates)
    {
        _vertices.at(i).texture_coordinates = coordinates;
        update_texture_coordinate();
        update_data(false, false, true);
    }

    Vector2f Shape::get_vertex_texture_coordinate(size_t i) const
    {
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
        update_data(false, true, false);
    }

    Vector2f Shape::get_centroid() const
    {
        Vector3f sum = Vector3f(0);
        for (auto& v : _vertices)
            sum += v.position;

        return sum / Vector3f(_vertices.size());
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

    void Shape::rotate(Angle angle)
    {
        auto transform = GLTransform();
        transform.rotate(angle, to_gl_position(get_centroid()));

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

    Texture* Shape::get_texture()
    {
        return _texture;
    }

    void Shape::set_texture(Texture* texture)
    {
        _texture = texture;
    }
}