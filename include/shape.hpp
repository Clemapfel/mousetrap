// 
// Copyright 2022 Clemens Cords
// Created on 7/16/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <mutex>

#include <include/gl_common.hpp>
#include <include/shader.hpp>
#include <include/colors.hpp>
#include <include/geometric_shapes.hpp>
#include <include/texture.hpp>

namespace mousetrap
{
    //
    class Shape
    {
        public:
            Shape();
            ~Shape();

            void as_triangle(Vector2f a, Vector2f b, Vector2f c);
            void as_rectangle(Vector2f top_left, Vector2f size);
            void as_rectangle(Vector2f, Vector2f, Vector2f, Vector2f);
            void as_circle(Vector2f center, float radius, size_t n_outer_vertices);
            void as_line(Vector2f a, Vector2f b);
            void as_line_strip(std::vector<Vector2f>);
            void as_polygon(std::vector<Vector2f> positions); // bounding polygon
            void as_wireframe(std::vector<Vector2f>);         // bounding polygon

            void as_frame(Vector2f top_left, Vector2f size, float x_width, float y_width);

            void render(Shader& shader, GLTransform transform);

            RGBA get_vertex_color(size_t) const;
            void set_vertex_color(size_t, RGBA);

            void set_vertex_texture_coordinate(size_t, Vector2f);
            Vector2f get_vertex_texture_coordinate(size_t) const;

            void set_vertex_position(size_t, Vector3f);
            Vector3f get_vertex_position(size_t) const;

            size_t get_n_vertices() const;

            void set_color(RGBA);

            Rectangle get_bounding_box() const;
            Vector2f get_size() const;
            void set_size(Vector2f);

            void set_centroid(Vector2f);
            Vector2f get_centroid() const;

            void set_top_left(Vector2f);
            Vector2f get_top_left() const;

            void rotate(Angle);

            void set_texture(Texture*);
            Texture* get_texture();

        protected:
            struct Vertex
            {
                Vertex(float x, float y)
                        : position(x, y, 0), color(1, 1, 1, 1), texture_coordinates(0, 0)
                {}

                Vector3f position;
                RGBA color;
                Vector2f texture_coordinates;
            };

            std::vector<Vertex> _vertices;
            std::vector<int> _indices;
            GLenum _render_type = GL_TRIANGLE_STRIP;

            void update_position();
            void update_color();
            void update_texture_coordinate();
            void initialize();

            std::vector<Vector2f> sort_by_angle(const std::vector<Vector2f>&);

        private:
            struct VertexInfo
            {
                float _position[3];
                float _color[4];
                float _texture_coordinates[2];
            };

            void update_data(
                    bool update_position = true,
                    bool update_color = true,
                    bool update_tex_coords = true
            );

            std::vector<VertexInfo> _vertex_data;

            GLNativeHandle _vertex_array_id = 0,
                    _vertex_buffer_id = 0;

            Texture* _texture = nullptr;
    };
}

#include <src/shape.inl>
