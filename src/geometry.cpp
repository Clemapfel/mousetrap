// 
// Copyright 2022 Clemens Cords
// Created on 8/14/22 by clem (mail@clemens-cords.com)
//

#include <include/geometry.hpp>

namespace mousetrap
{
    bool is_point_in_rectangle(Vector2f point, Rectangle rectangle)
    {
        return point.x >= rectangle.top_left.x and point.x <= rectangle.top_left.x + rectangle.size.x and
               point.y >= rectangle.top_left.y and point.y <= rectangle.top_left.y + rectangle.size.y;
    }

    bool intersecting(Line first, Line second, Vector2f* intersect)
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

    bool intersecting(Line line, Rectangle rectangle, std::vector<Vector2f>* intersections)
    {
        if (is_point_in_rectangle(line.a, rectangle) and is_point_in_rectangle(line.b, rectangle))
            return true;

        auto x = rectangle.top_left.x;
        auto y = rectangle.top_left.y;
        auto w = rectangle.size.x;
        auto h = rectangle.size.y;

        // abcd clockwise edges of rectangle

        Line ab = {{x, y}, {x + w, y}};
        Line bc = {{x + w, y}, {x + w, y + h}};
        Line cd = {{x + w, y + h}, {x, y + h}};
        Line da = {{x, y + h}, {x, y}};

        if (intersections != nullptr)
            intersections->clear();

        Vector2f to_push;

        if (intersecting(line, ab, &to_push))
        {
            if (intersections == nullptr)
                return true;

            intersections->emplace_back(to_push);
        }

        if (intersecting(line, bc, &to_push))
        {
            if (intersections == nullptr)
                return true;

            intersections->emplace_back(to_push);
        }

        if (intersecting(line, cd, &to_push))
        {
            if (intersections == nullptr)
                return true;

            intersections->emplace_back(to_push);
        }

        if (intersecting(line, da, &to_push))
        {
            if (intersections == nullptr)
                return true;

            intersections->emplace_back(to_push);
        }

        if (intersections == nullptr)
            return false;
        else
            return not intersections->empty();
    }
}