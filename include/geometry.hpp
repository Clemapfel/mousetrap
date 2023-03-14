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
    struct Triangle
    {
        Vector2f a, b, c;
    };

    struct Rectangle
    {
        Vector2f top_left;
        Vector2f size;
    };

    struct Circle
    {
        Vector2f center;
        float radius;
    };

    struct Line
    {
        Vector2f a, b;
    };

    bool is_point_in_rectangle(Vector2f point, Rectangle rectangle);

    bool intersecting(Line first, Line second, Vector2f* intersect = nullptr);
    bool intersecting(Line line, Rectangle rectangle, std::vector<Vector2f>* intersections = nullptr);
}
