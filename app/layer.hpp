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

            void draw_pixel(Vector2i, RGBA);
            void draw_image(Vector2i centroid, const Image&, RGBA multiplication = RGBA(1, 1, 1, 1));
            void draw_line(Vector2i start, Vector2i end, RGBA);

            void draw_rectangle_filled(Vector2i top_left, Vector2i bottom_right, RGBA);
            void draw_rectangle(Vector2i top_left, Vector2i bottom_right, RGBA, size_t px = 1);

            void draw_polygon(const std::vector<Vector2i>& points, RGBA);
            void draw_polygon_filled(const std::vector<Vector2i>& points, RGBA);

            /// \param aabb_top_left: bounding box top left pixel coordinate
            /// \param aabb_bottom_right: bounding box bottom right pixel coordinate
            void draw_ellipse(Vector2i aabb_top_left, Vector2i aabb_bottom_right, RGBA);
            void draw_ellipse_filled(Vector2i aabb_top_left, Vector2i aabb_bottom_right, RGBA);

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
        float x1 = a.x;
        float x2 = b.x;
        float y1 = a.y;
        float y2 = b.y;

        std::vector<Vector2i> out = {a, b};

        if (x2 == x1)
        {
            for (size_t y = std::min(y1, y2); y < std::max(y1, y2); ++y)
                out.push_back({x1, y});
            return out;
        }
        else if (y2 == y1)
        {
            for (size_t x = std::min(x1, x2); x < std::max(x1, x2); ++x)
                out.push_back({x, y1});
            return out;
        }

        float slope = (y2 - y1) / float(x2 - x1);
        float intercept = x1 * slope - y1;

        float eps = state::tooltips_file->get_value_as<float>("TODO", "line_eps");
        for (size_t x = std::min(x1, x2); x < std::max(x1, x2); ++x)
        {
            for (size_t y = std::min(y1, y2); y < std::max(y1, y2); ++y)
            {
                if (abs(y - x * slope + intercept) < eps)
                    out.push_back({x, y});
            }
        }

        return out;
    }

    void Layer::Frame::draw_pixel(Vector2i xy, RGBA color)
    {
        if (xy.x >= 0 and xy.x < image->get_size().x and xy.y >= 0 and xy.y < image->get_size().y)
            image->set_pixel(xy.x, xy.y, color);
    }

    void Layer::Frame::draw_image(Vector2i pos, const Image& image, RGBA multiplication)
    {
        static float alpha_eps = state::settings_file->get_value_as<float>("global", "alpha_epsilon");

        auto x_start = pos.x - image.get_size().x / 2;
        auto y_start = pos.y - image.get_size().y / 2;

        for (size_t x = x_start; x < x_start + image.get_size().x; ++x)
        {
            for (size_t y = y_start; y < y_start + image.get_size().y; ++y)
            {
                auto color = image.get_pixel(x - x_start, y - y_start);
                if (color.a > alpha_eps)
                    draw_pixel({x, y}, RGBA(
                        color.r * multiplication.r,
                        color.g * multiplication.g,
                        color.b * multiplication.b,
                        color.a * multiplication.a
                    ));
            }
        }
    }

    void Layer::Frame::draw_line(Vector2i start, Vector2i end, RGBA color)
    {
        for (auto& pos : get_line_points(start, end))
            draw_pixel(pos, color);
    }

    void Layer::Frame::draw_polygon(const std::vector<Vector2i>& points, RGBA color)
    {
        for (size_t i = 0; i < points.size(); ++i)
            for (auto& pos : get_line_points(points.at(i), points.at(i+1)))
                draw_pixel(pos, color);

        for (auto& pos : get_line_points(points.back(), points.front()))
            draw_pixel(pos, color);
    }

    void Layer::Frame::draw_polygon_filled(const std::vector<Vector2i>& points, RGBA color)
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
                draw_pixel({x, pair.first}, color);
    }

    void Layer::Frame::draw_rectangle(Vector2i top_left, Vector2i bottom_right, RGBA color, size_t px)
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
                draw_pixel({i, j}, color);

        for (auto i = x; i < x + w; ++i)
            for (auto j = y + h - y_m; j < y + h; ++j)
                draw_pixel({i, j}, color);

        for (auto i = x; y_m != h / 2 and i < x + x_m; ++i)
            for (auto j = y + y_m; j < y + h - y_m; ++j)
                draw_pixel({i, j}, color);

        for (auto i = x + w - x_m; y_m != h / 2 and i < x + w; ++i)
            for (auto j = y + y_m; j < y + h - y_m; ++j)
                draw_pixel({i, j}, color);
    }

    void Layer::Frame::draw_rectangle_filled(Vector2i top_left, Vector2i bottom_right, RGBA color)
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
                draw_pixel({x, y}, color);
    }

    void Layer::Frame::update_texture()
    {
        texture->create_from_image(*image);
    }
}
