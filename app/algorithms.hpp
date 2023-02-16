// 
// Copyright 2022 Clemens Cords
// Created on 12/8/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

namespace mousetrap
{
    /// \brief generate 1-pixel rasterized line between two texels
    std::vector<Vector2i> generate_line_points(Vector2i a, Vector2i b);

    /// \brief generate 1-pixel rasterized circle outline
    std::vector<Vector2i> generate_circle_points(size_t width, size_t height);

    std::vector<Vector2i> generate_rectangle_points(Vector2i top_left, size_t width, size_t height);

    struct OutlineVertices
    {
        std::vector<std::pair<Vector2i, Vector2i>> top, right, bottom, left;
    };

    /// \brief generate lines that will outline the non-0 area of an image, ordered by clockwise orientation
    OutlineVertices generate_outline_vertices(const Image& image);

    /// \brief generate lines that will outline the area described by the set of coordinates, ordered by clockwise orientation
    OutlineVertices generate_outline_vertices(const Vector2Set<int>& set);

    Image generate_rectangle_outline(size_t width, size_t height, HSVA color = HSVA(0, 1, 0, 1));
    Image generate_rectangle_filled(size_t width, size_t height, HSVA color = HSVA(0, 1, 0, 1));
    Image generate_circle_outline(size_t width, size_t height, HSVA color = HSVA(0, 1, 0, 1));
    Image generate_circle_filled(size_t width, size_t height, HSVA color = HSVA(0, 1, 0, 1));

    [[nodiscard]] Image rotate_image_clockwise(const Image&);
    [[nodiscard]] Image rotate_image_counter_clockwise(const Image&);
    [[nodiscard]] Image flip_image_horizontally(const Image&);
    [[nodiscard]] Image flip_image_vertically(const Image&);

}