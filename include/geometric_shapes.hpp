// 
// Copyright 2022 Clemens Cords
// Created on 6/27/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gl_common.hpp>

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

    bool is_point_in_rectangle(Vector2f point, Rectangle rectangle)
    {
        return point.x >= rectangle.top_left.x and point.x <= rectangle.top_left.x + rectangle.size.x and
               point.y >= rectangle.top_left.y and point.y <= rectangle.top_left.y + rectangle.size.y;
    }

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

    bool intersecting(Line line, Rectangle rectangle, std::vector<Vector2f>* intersections = nullptr)
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

    std::vector<Vector2i> smooth_line(const std::vector<Vector2i>& in)
    {
        std::vector<Vector2i> out;

        auto is_present = [&](int x, int y) -> bool {
            return std::find(out.begin(), out.end(), Vector2i(x, y)) != out.end();
        };

        for (auto& pos : in)
        {
            int x = pos.x;
            int y = pos.y;

            bool north = is_present(x, y - 1);
            bool north_east = is_present(x + 1, y - 1);
            bool east = is_present(x + 1, y);
            bool south_east = is_present(x + 1, y + 1);
            bool south = is_present(x, y + 1);
            bool south_west = is_present(x - 1, y + 1);
            bool west = is_present(x - 1, y);
            bool north_west = is_present(x - 1, y + 1);

            if (north or east or south or west)
                continue;

            out.push_back({x, y});
        }

        if (out.empty())
            return out;

        std::vector<Vector2i> fill_in;
        for (size_t i = 0; i < out.size() - 1; ++i)
        {
           auto current = out.at(i);
           auto next = out.at(i+1);

           if (abs(current.x - next.x) == 1 or abs(current.y - next.y) == 1)
               continue;
           else
           {
               Vector2i to_push = current;

                if (next.x == current.x)
                {
                    if (next.y == current.y + 2)
                        to_push = {current.x, current.y + 1};
                    else if (next.y == current.y - 2)
                        to_push = {current.x, current.y - 1};
                }
                else if (next.y == current.y)
                {
                    if (next.x == current.x + 2)
                        to_push = {current.x + 1, current.y};
                    else if (next.x == current.y - 2)
                        to_push = {current.x - 1, current.y};
                }
                else
                {
                    if (next.x > current.x)
                    {
                        if (next.y < current.y)
                            to_push = {current.x + 1, current.y - 1};
                        else if (next.y > current.y)
                            to_push = {current.x + 1, current.y + 1};
                    }
                    else if (next.x < current.x)
                    {
                        if (next.y < current.y)
                            to_push = {current.x - 1, current.y - 1};
                        else if (next.y > current.y)
                            to_push = {current.x - 1, current.y + 1};
                    }
                }

               fill_in.push_back(to_push);
           }
        }

        for (auto& v : fill_in)
            out.push_back(v);

        return out;
    }
}