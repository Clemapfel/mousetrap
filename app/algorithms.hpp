// 
// Copyright 2022 Clemens Cords
// Created on 12/8/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

namespace mousetrap
{
    /// \brief generate 1-pixel rasterized line between two texels
    std::vector<Vector2i> generate_line_points(Vector2i a, Vector2i b)
    {
        std::vector<Vector2i> out = {a, b};

        if (a.x == b.x)
        {
            auto y_min = std::min(a.y, b.y);
            auto y_max = std::max(a.y, b.y);
            out.reserve(y_max - y_min);

            for (int y = y_min; y < y_max; ++y)
                out.push_back({a.x, y});

            return out;
        }
        else if (a.y == b.x)
        {
            auto x_min = std::min(a.x, b.x);
            auto x_max = std::max(a.x, b.x);
            out.reserve(x_max - x_min);

            for (int x = x_min; x < x_max; ++x)
                out.push_back({x, a.y});

            return out;
        }

        // source:
        //  [1] https://en.wikipedia.org/wiki/Digital_differential_analyzer_(graphics_algorithm)
        //  [2] https://www.cs.virginia.edu/luther/blog/posts/492.html

        float x1 = a.x;
        float x2 = b.x;
        float y1 = a.y;
        float y2 = b.y;

        float dx = x2 - x1;
        float dy = y2 - y1;
        float slope = dy / dx;

        const int eps = 10e7; // project into int range to avoid float precision resulting in non-deterministic results

        if (abs(dx) > abs(dy))
        {
            float x_step = x1 < x2 ? 1 : -1;
            float y_step = slope * x_step;

            float y = y1;
            float x = x1;

            auto n_steps = std::max(abs(dx), abs(dy));
            out.reserve(out.size() + n_steps);

            for (size_t step = 0; step < n_steps; ++step)
            {
                if (int(glm::fract(y) * eps) >= eps / 2)
                    out.emplace_back(x, int(y+1));
                else
                    out.emplace_back(x, int(y));

                x += x_step;
                y += y_step;
            }
        }
        else if (abs(dx) < abs(dy))
        {
            float y_step = y1 < y2 ? 1 : -1;
            float x_step = (1 / slope) * y_step;

            float x = x1;
            float y = y1;

            auto n_steps = std::max(abs(dx), abs(dy));
            out.reserve(out.size() + n_steps);

            for (size_t step = 0; step < n_steps; ++step)
            {
                if (int(glm::fract(x) * eps) >= eps / 2)
                    out.emplace_back(int(x+1), y);
                else
                    out.emplace_back(int(x), y);

                x += x_step;
                y += y_step;
            }
        }
        else
        {
            int x = a.x;
            int y = a.y;

            int x_step = a.x < b.x ? 1 : -1;
            int y_step = a.y < b.y ? 1 : -1;

            size_t n_steps = abs(a.x - b.x); // same as abs(b.y - a.y)
            out.reserve(out.size() + n_steps);

            for (size_t i = 0; i < n_steps; ++i)
            {
                out.emplace_back(x, y);
                x += x_step;
                y += y_step;
            }
        }


        return out;
    }

    struct OutlineVertices
    {
        std::vector<std::pair<Vector2i, Vector2i>> top, right, bottom, left;
    };

    /// \brief generate lines that will outline the non-0 area of an image, ordered by clockwise orientation
    OutlineVertices generate_outline_vertices(const Image& image)
    {
        OutlineVertices out;

        auto w = image.get_size().x;
        auto h = image.get_size().y;

        static auto alpha_eps = state::settings_file->get_value_as<float>("global", "alpha_epsilon");

        // hlines
        for (size_t x = 0; x < w; ++x)
        {
            for (size_t y = 0; y < h - 1; ++y)
            {
                auto top = image.get_pixel(x, y).a > alpha_eps;
                auto bottom = image.get_pixel(x, y + 1).a > alpha_eps;

                std::pair<Vector2i, Vector2i> to_push = {
                    {x,     y + 1},
                    {x + 1, y + 1}
                };

                if (top and not bottom)
                    out.left.push_back(to_push);
                else if (not top and bottom)
                    out.right.push_back(to_push);
            }
        }

        for (size_t x = 0; x < w; ++x)
            if (image.get_pixel(x, 0).a > alpha_eps)
                out.left.push_back({{x, 0}, {x + 1, 0}});

        for (size_t x = 0; x < w; ++x)
            if (image.get_pixel(x, h-1).a > alpha_eps)
                out.right.push_back({{x, h}, {x+1, h}});

        // vlines
        for (size_t y = 0; y < h; ++y)
        {
            for (size_t x = 0; x < w - 1; ++x)
            {
                auto left = image.get_pixel(x, y).a > alpha_eps;
                auto right = image.get_pixel(x + 1, y).a > alpha_eps;

                std::pair<Vector2i, Vector2i> to_push = {
                    {x + 1, y},
                    {x + 1, y + 1}
                };

                if (left and not right)
                    out.top.push_back(to_push);
                else if (not left and right)
                    out.bottom.push_back(to_push);
            }
        }

        for (size_t y = 0; y < h; ++y)
            if (image.get_pixel(0, y).a > alpha_eps)
                out.top.push_back({{0, y}, {0, y+1}});

        for (size_t y = 0; y < h; ++y)
            if (image.get_pixel(w-1, y).a > alpha_eps)
                out.bottom.push_back({{w, y}, {w, y+1}});

        return out;
    }
}