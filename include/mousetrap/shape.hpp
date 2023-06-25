//
// Copyright 2022 Clemens Cords
// Created on 7/16/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <mutex>

#include <mousetrap/gl_common.hpp>
#include <mousetrap/shader.hpp>
#include <mousetrap/color.hpp>
#include <mousetrap/gl_transform.hpp>
#include <mousetrap/texture.hpp>
#include <mousetrap/geometry.hpp>
#include <mousetrap/signal_emitter.hpp>

namespace mousetrap
{
    /// @brief render shape vertex
    struct Vertex
    {
        /// @brief constructor
        /// @param x x-coordiante, in gl coordinates
        /// @param y y-coordinate, in gl coordinates
        /// @param color
        Vertex(float x, float y, RGBA color)
            : position(x, y, 0), color(color), texture_coordinates(0, 0)
        {}

        /// @brief position in 3d space, gl coordinates
        Vector3f position;

        /// @brief color
        RGBA color;

        /// @brief texture coordinates, in relative image coordinates
        Vector2f texture_coordinates;
    };

    #ifndef DOXYGEN
    class Shape;
    namespace detail
    {
        struct VertexInfo
        {
            float _position[3];
            float _color[4];
            float _texture_coordinates[2];
        };

        enum class ShapeType
        {
            UNKNOWN,
            POINT,
            POINTS,
            TRIANGLE,
            RECTANGLE,
            CIRCLE,
            ELLIPSE,
            LINE,
            LINES,
            LINE_STRIP,
            POLYGON,
            RECTANGULAR_FRAME,
            CIRCULAR_RING,
            ELLIPTICAL_RING,
            WIREFRAME,
            OUTLINE
        };

        struct _ShapeInternal
        {
            GObject parent;

            RGBA* color;
            bool is_visible = true;

            std::vector<Vertex>* vertices;
            std::vector<int>* indices;
            GLenum render_type = GL_TRIANGLE_STRIP;
            ShapeType shape_type = ShapeType::UNKNOWN;

            std::vector<VertexInfo>* vertex_data;

            GLNativeHandle vertex_array_id = 0;
            GLNativeHandle vertex_buffer_id = 0;

            const TextureObject* texture = nullptr;
        };
        using ShapeInternal = _ShapeInternal;
        DEFINE_INTERNAL_MAPPING(Shape);
    }
    #endif

    /// @brief opengl shape primitive, variable number of vertices
    class Shape : public detail::notify_if_gl_uninitialized,
        public SignalEmitter
    {
        public:
            /// @brief ctor, allocates vertex buffer gpu-side
            Shape();

            /// @brief dtor, deallocated vertex buffer gpu-side
            ~Shape();

            /// @brief construct from internal, \for_internal_use_only
            /// @param internal
            Shape(detail::ShapeInternal*);

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief copy ctor, allocates new vertex array with identical vertex data
            /// @param other
            Shape(const Shape&);

            /// @brief copy assignment, allocates new vertex array with identical vertex data
            /// @param other
            /// @returns reference to self after assignment
            Shape& operator=(const Shape&);

            /// @brief move ctor
            /// @param other
            Shape(Shape&&) noexcept;

            /// @brief move assignment
            /// @param other
            /// @returns reference to self after assignment
            Shape& operator=(Shape&&) noexcept;

            /// @brief access OpenGL vertex buffer id
            /// @returns id
            GLNativeHandle get_native_handle() const;

            /// @brief construct as 2d point that point is always rendered as exactly 1 fragment
            /// @param position position gl coordinates
            void as_point(Vector2f position);

            /// @copydoc Shape::as_point
            static Shape Point(Vector2f position);

            /// @brief construct as set of points, each point is always rendered as exactly 1 fragment
            /// @param points vector of points
            void as_points(const std::vector<Vector2f>& points);

            /// @copydoc Shape::as_points
            static Shape Points(const std::vector<Vector2f>& points);

            /// @brief construct as filled triangle
            /// @param a point in gl coordinates
            /// @param b point in gl coordinates
            /// @param c point in gl coordinates
            void as_triangle(Vector2f a, Vector2f b, Vector2f c);

            /// @copydoc Shape::as_triangle
            static Shape Triangle(Vector2f a, Vector2f b, Vector2f c);

            /// @brief construct as filled rectangle from top left and size
            /// @param top_left point in gl coordinates
            /// @param size length in gl coordinates
            void as_rectangle(Vector2f top_left, Vector2f size);

            /// @copydoc Shape::as_rectangle(Vector2f top_left, Vector2f size)
            static Shape Rectangle(Vector2f top_left, Vector2f size);

            /// @brief construct as filled circle
            /// @param center point in gl coordinates
            /// @param radius distance in gl coordinates
            /// @param n_outer_vertices number of equally spaced vertices on the perimeter of the circle
            void as_circle(Vector2f center, float radius, size_t n_outer_vertices);

            /// @copydoc Shape::as_circle
            static Shape Circle(Vector2f center, float radius, size_t n_outer_vertices);

            /// @brief construct as ellipse
            /// @param center point in gl coordinates
            /// @param x_radius radius along the x-axis, normalized distance in 2d space
            /// @param y_radius radius along the y-axis, normalized distance in 2d space
            /// @param n_outer_vertices number of equally spaced vertices on the perimeter of the circle
            void as_ellipse(Vector2f center, float x_radius, float y_radius, size_t n_outer_vertices);

            /// @copydoc Shape::as_ellipse
            static Shape Ellipse(Vector2f center, float x_radius, float y_radius, size_t n_outer_vertices);

            /// @brief construct as line, has a width of 1 fragment exactly
            /// @param a point in gl coordinates
            /// @param b point in gl coordinates
            void as_line(Vector2f a, Vector2f b);

            /// @copydoc Shape::as_line
            static Shape Line(Vector2f a, Vector2f b);

            /// @brief construct as set of lines, each has a width of exactly 1 fragment
            /// @param points vector of pairs of 2 points, both in gl coordinates
            void as_lines(const std::vector<std::pair<Vector2f, Vector2f>>& points);

            /// @copydoc Shape::as_lines
            static Shape Lines(const std::vector<std::pair<Vector2f, Vector2f>>& points);

            /// @brief construct as set of connected lines
            /// @param points {a1, a2, ..., an} will result in line segments {a1, a2}, {a2, a3}, ..., {an-1, an}
            void as_line_strip(const std::vector<Vector2f>& points);

            /// @copydoc Shape::as_line_strip
            static Shape LineStrip(const std::vector<Vector2f>& points);

            /// @brief construct as convex polygon
            /// @param points points in gl coordinates, minimum bounding polygon is calculated on these, so some of the vertices ay be discarded
            void as_polygon(const std::vector<Vector2f>& points);

            /// @copydoc Shape::as_polygon
            static Shape Polygon(const std::vector<Vector2f>& points);

            /// @brief construct as rectanglular frame of given thickness
            /// @param top_left top left anchor of the outer perimeter of the frame, in gl coordinates
            /// @param outer_size width and height of the oute perimeter of the frame, in gl coordinates
            /// @param x_width horizontal width of the frames inner bounds, in gl coordinates
            /// @param y_width vertical height of the frames inner bounds, in gl coordinates
            void as_rectangular_frame(Vector2f top_left, Vector2f outer_size, float x_width, float y_width);

            /// @copydoc Shape::as_rectangle_frame
            static Shape RectangularFrame(Vector2f top_left, Vector2f outer_size, float x_width, float y_width);

            /// @brief construct as circular ring, a "2d donut"
            /// @param center center in 2d space
            /// @param outer_radius radius from the center to the outer perimeter of the ring
            /// @param thickness width of the rings inner bounds along all axis
            /// @param n_outer_vertices number of vertices on the outer perimeter of the ring
            void as_circular_ring(Vector2f center, float outer_radius, float thickness, size_t n_outer_vertices);

            /// @copydoc Shape::as_circular_ring
            static Shape CircularRing(Vector2f center, float outer_radius, float thickness, size_t n_outer_vertices);

            /// @brief construct as elliptic ring, a "2d elliptic donut"
            /// @param center center in 2d space
            /// @param x_radius horizontal radius of the outer perimeter of the ellipse
            /// @param y_radius vertical radius of the outer perimeter of the ellipse
            /// @param x_thickness width of the inner bound of the ellipse along the x-axis
            /// @param y_thickness height of the inner bounds of the ellipse along the y-axis
            /// @param n_outer_vertices number of vertices on the outer perimeter of the ellipse
            void as_elliptical_ring(Vector2f center, float x_radius, float y_radius, float x_thickness, float y_thickness, size_t n_outer_vertices);

            /// @copydoc Shape::as_elliptic_ring
            static Shape EllipticalRing(Vector2f center, float x_radius, float y_radius, float x_thickness, float y_thickness, size_t n_outer_vertices);

            /// @brief construct as a closed loop linesegment
            /// @param points {a1, a2, ..., an} will result in line segments {a1, a2}, {a2, a3}, ..., {an-1, an}, {an, a1}
            void as_wireframe(const std::vector<Vector2f>& points);

            /// @copydoc Shape::as_wireframe
            static Shape Wireframe(const std::vector<Vector2f>& points);

            /// @brief construct a wireframe from a shapes outer vertices. Useful for generating frames or outlines
            /// @param shape another shape, will generate minimum bounding polygon and construct a wireframe from that polygon
            void as_outline(const Shape& shape, RGBA color = RGBA(0, 0, 0, 1));

            /// @copydoc Shape::as_outline
            static Shape Outline(const Shape& shape);

            /// @brief render the shape to the currently bound framebuffer
            /// @param shader shader program to use
            /// @param transform transform to hand to the vertex shader
            void render(const Shader& shader, GLTransform transform) const;

            /// @brief get color of n-th vertex, returns RGBA(0, 0, 0, 0) and prints a warning if vertex index out of bounds
            /// @param index vertex index
            /// @returns color as RGBA
            RGBA get_vertex_color(size_t index) const;

            /// @brief set color of n-th vertex, does nothing if vertex index out of bounds
            /// @param index vertex index
            /// @param new_color
            void set_vertex_color(size_t index, RGBA new_color);

            /// @brief set texture coordinate of vertex, does nothing if vertex index out of bounds
            /// @param index vertex index
            /// @param coordinate new texture coordinate, {0, 0} for top left, {1, 1} for bottom right of texture
            void set_vertex_texture_coordinate(size_t index, Vector2f coordinate);

            /// @brief get vertex texture coordinate, returns Vector2f() if index out of bounds
            /// @param index vertex index
            /// @return texture coordinate, {0, 0} for top left, {1, 1} for bottom right of texture
            Vector2f get_vertex_texture_coordinate(size_t index) const;

            /// @brief set vertex position in 3d space, does nothing if index out of bounds
            /// @param index vertex index
            /// @param position position in 3d space
            /// @note if multiple vertex positions change at the same time, use mousetrap::Vertex::as_rectangle (or other appropriate shape) to update all of them at once in a more performant manned
            void set_vertex_position(size_t index, Vector3f position);

            /// @brief get vertex position in 3d space, return Vector3f() if out of bounds
            /// @param index vertex index
            /// @return position in 3d space
            Vector3f get_vertex_position(size_t index) const;

            /// @brief get number of vertices, the number depends on the shape and may be larger than intuitive
            /// @return number of vertices
            size_t get_n_vertices() const;

            /// @brief set color of all vertices at once
            /// @param rgba color in RGBA
            void set_color(RGBA rgba);

            /// @brief set whether shape should render when instructed
            /// @param b if false, mousetrap::Shape::render will do nothing, true otherwise
            void set_is_visible(bool b);

            /// @brief get whether shape should render when instructed
            /// @return true if false, mousetrap::Shape::render will do nothing, true otherwise
            bool get_is_visible() const;

            /// @brief get axis aligned bounding box of all vertices
            /// @return rectangle
            struct Rectangle get_bounding_box() const;

            /// @brief get size of axis aligned bounding box
            /// @return width, height
            Vector2f get_size() const;

            /// @brief align all vertices such that the centroid, the center of the axis aligned bounding box, is set to the given position
            /// @param new_position
            void set_centroid(Vector2f new_position);

            /// @brief get centroid, center of the axis aligned bounding box
            /// @return position
            Vector2f get_centroid() const;

            /// @brief align top left of axis aligned bounding box with position
            /// @param position
            void set_top_left(Vector2f position);

            /// @brief get top left of axis aligned bounding box
            /// @return position
            Vector2f get_top_left() const;

            /// @brief rotate all vertices around origin
            /// @param angle
            /// @param origin point in 2d space
            void rotate(Angle angle, Vector2f origin);

            /// @brief set texture of shape, has to be queried in the fragment shader using the <tt>int _texture_set</tt> and <tt>Sampler2D _texture</tt> uniforms, the default fragment shader does this automatically
            /// @param texture texture object, such as mousetrap::Texture, mousetrap::RenderTexture or mousetrap::MultisampledRenderTexture. The user is responsible for making sure the texture stays in memory. May be nullptr
            void set_texture(const TextureObject* texture);

            /// @brief get texture object
            /// @returns pointer to texture object, or nullptr if no texture is registered
            const TextureObject* get_texture() const;

            /// @brief expose as GObject
            operator NativeObject() const override;

        private:
            void update_position() const;
            void update_color() const;
            void update_texture_coordinate() const;
            void initialize();

            std::vector<Vector2f> sort_by_angle(const std::vector<Vector2f>&);

            void update_data(
                bool update_position = true,
                bool update_color = true,
                bool update_tex_coords = true
            ) const;

            detail::ShapeInternal* _internal = nullptr;
    };
}

