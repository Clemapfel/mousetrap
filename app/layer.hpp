// 
// Copyright 2022 Clemens Cords
// Created on 9/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/texture.hpp>

#include <app/global_state.hpp>
#include <app/brush.hpp>
#include <app/settings_files.hpp>

namespace mousetrap
{
    std::vector<Vector2i> get_line_points(Vector2i start, Vector2i end);

    struct Layer
    {
        std::string name;

        struct Frame
        {
            Image* image;
            Texture* texture;
            bool is_tween_repeat = false;

            void draw_pixel(Vector2i, RGBA, BlendMode = BlendMode::NORMAL);
            void draw_image(Vector2i centroid, const Image&, RGBA multiplication = RGBA(1, 1, 1, 1), BlendMode = BlendMode::NORMAL);
            void draw_line(Vector2i start, Vector2i end, RGBA, BlendMode = BlendMode::NORMAL);

            void draw_rectangle_filled(Vector2i top_left, Vector2i bottom_right, RGBA, BlendMode = BlendMode::NORMAL);
            void draw_rectangle(Vector2i top_left, Vector2i bottom_right, RGBA, size_t px = 1, BlendMode = BlendMode::NORMAL);

            void draw_polygon(const std::vector<Vector2i>& points, RGBA, BlendMode = BlendMode::NORMAL);
            void draw_polygon_filled(const std::vector<Vector2i>& points, RGBA, BlendMode = BlendMode::NORMAL);

            /// \param aabb_top_left: bounding box top left pixel coordinate
            /// \param aabb_bottom_right: bounding box bottom right pixel coordinate
            void draw_ellipse(Vector2i aabb_top_left, Vector2i aabb_bottom_right, RGBA, BlendMode = BlendMode::NORMAL);
            void draw_ellipse_filled(Vector2i aabb_top_left, Vector2i aabb_bottom_right, RGBA, BlendMode = BlendMode::NORMAL);

            void update_texture();
        };

        std::deque<Frame> frames = std::deque<Frame>();

        bool is_locked = false;
        bool is_visible = true;
        float opacity = 1;
        BlendMode blend_mode = NORMAL;
    };
}

/// ###

namespace mousetrap
{
    std::vector<Vector2i> get_line_points(Vector2i a, Vector2i b)
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

    void Layer::Frame::draw_pixel(Vector2i xy, RGBA color, BlendMode blend_mode)
    {
        if (not (xy.x >= 0 and xy.x < image->get_size().x and xy.y >= 0 and xy.y < image->get_size().y))
            return;

        auto dest = image->get_pixel(xy.x, xy.y);
        auto source = color;
        RGBA final;


        bool should_clamp = false;

        switch (blend_mode)
        {
            case BlendMode::NONE:
                final = dest;
                break;

            case BlendMode::NORMAL:
            {
                /*
                float alpha = source.a + source.b * (1 - source.a);
                auto blend = [&](float a, float b) -> float {
                    return (a * source.a + b * dest.a * (1 - source.a)) / alpha;
                };

                final.r = blend(source.r, dest.r);
                final.g = blend(source.g, dest.g);
                final.b = blend(source.b, dest.b);
                final.a = glm::mix(source.a, dest.a, 0.5);
                */

                final.r = source.r + dest.r * (1 - source.a);
                final.g = source.g + dest.g * (1 - source.a);
                final.b = source.b + dest.b * (1 - source.a);
                final.a = source.a + dest.a * (1 - source.a);

                if (final.r > source.r)
                    final.r = source.r;

                if (final.g > source.g)
                    final.g = source.g;

                if (final.b > source.b)
                    final.b = source.b;

                break;
            }

            case BlendMode::SUBTRACT:
                final.r = source.r - dest.r;
                final.g = source.g - dest.g;
                final.b = source.b - dest.b;
                final.a = source.a - dest.a;

                should_clamp = true;
                break;

            case BlendMode::MULTIPLY:
                final.r = source.r * dest.r;
                final.g = source.g * dest.g;
                final.b = source.b * dest.b;
                final.a = source.a * dest.a;

                should_clamp = true;
                break;

            case BlendMode::REVERSE_SUBTRACT:
                final.r = dest.r - source.r;
                final.g = dest.g - source.g;
                final.b = dest.b - source.b;
                final.a = dest.a - source.a;

                should_clamp = true;
                break;

            case BlendMode::ADD:
                final.r = source.r + dest.r;
                final.g = source.g + dest.g;
                final.b = source.b + dest.b;
                final.a = source.a + dest.a;

                should_clamp = true;
                break;

            case BlendMode::MAX:
                final.r = std::max<float>(source.r, dest.r);
                final.g = std::max<float>(source.g, dest.g);
                final.b = std::max<float>(source.b, dest.b);
                final.a = std::max<float>(source.a, dest.a);
                break;

            case BlendMode::MIN:
                final.r = std::min<float>(source.r, dest.r);
                final.g = std::min<float>(source.g, dest.g);
                final.b = std::min<float>(source.b, dest.b);
                final.a = std::min<float>(source.a, dest.a);
                break;
        }

        if (should_clamp)
        {
            final.r = glm::clamp<float>(final.r, 0, 1);
            final.g = glm::clamp<float>(final.g, 0, 1);
            final.b = glm::clamp<float>(final.b, 0, 1);
            final.a = glm::clamp<float>(final.a, 0, 1);
        }

        image->set_pixel(xy.x, xy.y, final);
    }

    void Layer::Frame::draw_line(Vector2i start, Vector2i end, RGBA color, BlendMode blend_mode)
    {
        for (auto& pos : get_line_points(start, end))
            draw_pixel(pos, color, blend_mode);
    }

    void Layer::Frame::draw_polygon(const std::vector<Vector2i>& points, RGBA color, BlendMode blend_mode)
    {
        for (size_t i = 0; i < points.size(); ++i)
            for (auto& pos : get_line_points(points.at(i), points.at(i+1)))
                draw_pixel(pos, color, blend_mode);

        for (auto& pos : get_line_points(points.back(), points.front()))
            draw_pixel(pos, color, blend_mode);
    }

    void Layer::Frame::draw_polygon_filled(const std::vector<Vector2i>& points, RGBA color, BlendMode blend_mode)
    {
        // degenerate area into horizontal bands

        struct Bounds
        {
            int64_t min;
            int64_t max;
        };

        std::map<int, Bounds> coords;
        auto add_point = [&](const Vector2i& pos) -> void
        {
            auto it = coords.find(pos.y);
            if (it == coords.end())
                coords.insert({pos.y, Bounds{pos.x, pos.x}});
            else
            {
                auto& bounds = it->second;
                bounds.min = std::min<int>(bounds.min, pos.x);
                bounds.max = std::max<int>(bounds.max, pos.x);
            }
        };

        for (size_t i = 0; i < points.size(); ++i)
            for (auto& pos : get_line_points(points.at(i), points.at(i+1)))
                add_point(pos);

        for (auto& pos : get_line_points(points.back(), points.front()))
            add_point(pos);

        for (auto& pair : coords)
            for (size_t x = pair.second.min; x < pair.second.max; ++x)
                draw_pixel({x, pair.first}, color, blend_mode);
    }

    void Layer::Frame::draw_rectangle(Vector2i top_left, Vector2i bottom_right, RGBA color, size_t px, BlendMode blend_mode)
    {
        // guaranteed to only call draw_pixel the minimum number of times, no matter the input

        auto min_x = std::min(top_left.x, bottom_right.x);
        auto min_y = std::min(top_left.y, bottom_right.y);
        auto max_x = std::max(top_left.x, bottom_right.x);
        auto max_y = std::max(top_left.y, bottom_right.y);

        if (min_x < 0)
            min_x = 0;

        if (min_y < 0)
            min_y = 0;

        if (max_x >= image->get_size().x - 1)
            max_x = image->get_size().x - 1;

        if (max_y >= image->get_size().y - 1)
            max_y = image->get_size().y - 1;

        auto x = min_x;
        auto y = min_y;
        auto w = max_x - min_x;
        auto h = max_y - min_y;
        auto x_m = std::min<size_t>(px, w / 2);
        auto y_m = std::min<size_t>(px, h / 2);

        for (auto i = x; i < x + w; ++i)
            for (auto j = y; j < y + y_m; ++j)
                draw_pixel({i, j}, color, blend_mode);

        for (auto i = x; i < x + w; ++i)
            for (auto j = y + h - y_m; j < y + h; ++j)
                draw_pixel({i, j}, color, blend_mode);

        for (auto i = x; y_m != h / 2 and i < x + x_m; ++i)
            for (auto j = y + y_m; j < y + h - y_m; ++j)
                draw_pixel({i, j}, color, blend_mode);

        for (auto i = x + w - x_m; y_m != h / 2 and i < x + w; ++i)
            for (auto j = y + y_m; j < y + h - y_m; ++j)
                draw_pixel({i, j}, color, blend_mode);
    }

    void Layer::Frame::draw_rectangle_filled(Vector2i top_left, Vector2i bottom_right, RGBA color, BlendMode blend_mode)
    {
        auto min_x = std::min(top_left.x, bottom_right.x);
        auto min_y = std::min(top_left.y, bottom_right.y);
        auto max_x = std::max(top_left.x, bottom_right.x);
        auto max_y = std::max(top_left.y, bottom_right.y);

        if (min_x < 0)
            min_x = 0;

        if (min_y < 0)
            min_y = 0;

        if (max_x >= image->get_size().x - 1)
            max_x = image->get_size().x - 1;

        if (max_y >= image->get_size().y - 1)
            max_y = image->get_size().y - 1;

        for (size_t x = min_x; x < max_x; ++x)
            for (size_t y = min_y; y < max_y; ++y)
                draw_pixel({x, y}, color, blend_mode);
    }

    void Layer::Frame::update_texture()
    {
        texture->create_from_image(*image);
    }
}
