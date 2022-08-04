// 
// Copyright 2022 Clemens Cords
// Created on 6/27/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gl_common.hpp>

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

    bool intersecting(Line first, Line second, Vector2f* intersect = nullptr)
    {
        // source: https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect

        auto p0_x = first.a.x;
        auto p0_y = first.a.y;
        auto p1_x = first.b.x;
        auto p1_y = first.b.y;
        auto p2_x = second.a.x;
        auto p2_y = second.a.y;
        auto p3_x = second.b.x;
        auto p3_y = second.b.y;

        float s1_x, s1_y, s2_x, s2_y;
        s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
        s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

        float s, t;
        s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
        t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

        if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
        {
            if (intersect != nullptr)
            {
                intersect->x = p0_x + (t * s1_x);
                intersect->y = p0_y + (t * s1_y);
            }
            return true;
        }
        return false;
    }
}