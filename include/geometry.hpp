//
// Copyright 2022 Clemens Cords
// Created on 6/27/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gl_common.hpp>

#include <vector>
#include <unordered_set>

namespace mousetrap
{
    /// @brief a triangle, defined by three points in 2d space
    struct Triangle
    {
        /// @brief point
        Vector2f a;

        /// @brief point
        Vector2f b;

        /// @brief point
        Vector2f c;
    };

    /// @brief rectangle, defined by the top left corner in 2d space, width and height
    struct Rectangle
    {
        /// @brief top left corner
        Vector2f top_left;

        /// @brief width, height
        Vector2f size;

        /// @brief get top left
        /// @return position
        Vector2f get_top_left() const;

        /// @brief move rectangle such that top left is as specified
        /// @param position
        void set_top_left(Vector2f);

        /// @brief get center top
        /// @return position
        Vector2f get_center_top() const;

        /// @brief move rectangle such that top center is as specified
        /// @param position
        void set_top_center(Vector2f);

        /// @brief get top right
        /// @return position
        Vector2f get_top_right() const;

        /// @brief move rectangle such that top right is as specified
        /// @param position
        void set_top_right(Vector2f);

        /// @brief get center left
        /// @return position
        Vector2f get_center_left() const;

        /// @brief move rectangle such that center left is as specified
        /// @param position
        void set_center_left(Vector2f);

        /// @brief get center
        /// @return position
        Vector2f get_center() const;

        /// @brief move rectangle such that center is as specified
        /// @param position
        void set_center(Vector2f);

        /// @brief get center right
        /// @return position
        Vector2f get_center_right() const;

        /// @brief move rectangle such that center right is as specified
        /// @param position
        void set_center_right(Vector2f);

        /// @brief get bottom right
        /// @return position
        Vector2f get_bottom_right() const;

        /// @brief move rectangle such that bottom right is as specified
        /// @param position
        void set_bottom_right(Vector2f);

        /// @brief get center bottom
        /// @return position
        Vector2f get_center_bottom() const;

        /// @brief move rectangle such that center bottom is as specified
        /// @param position
        void set_center_bottom(Vector2f);

        /// @brief get bottom left
        /// @return position
        Vector2f get_bottom_left() const;

        /// @brief move rectangle such that bottom left is as specified
        /// @param position
        void set_bottom_left(Vector2f);
    };

    /// @brief circle, defined by center in 2d space, radius
    struct Circle
    {
        /// @brief center
        Vector2f center;

        /// @brief radius
        float radius;
    };

    /// @brief line, defined by two points in 2d space
    struct Line
    {
        /// @brief point
        Vector2f a;

        /// @brief point
        Vector2f b;
    };

    /// @brief test if point contained in rectangle, including wether it intersects with the rectangles outline
    /// @param point
    /// @param rectangle
    /// @return true if intersecting, false otherwise
    bool is_point_in_rectangle(Vector2f point, Rectangle rectangle);

    /// @brief test if two lines intersect
    /// @param first line
    /// @param second line
    /// @param intersect [out] intersection points, set if lines intersect, nullptr otherwise
    /// @return true if intersecting, false otherwise
    bool intersecting(Line first, Line second, Vector2f* intersect = nullptr);

    /// @brief test if a line and a rectangle intersect
    /// @param line
    /// @param rectangle
    /// @param intersections [out] number of intersection points, filled if lines intersect, nullptr otherwise
    /// @return true if intersecting, false otherwise
    bool intersecting(Line line, Rectangle rectangle, std::vector<Vector2f>* intersections = nullptr);
}