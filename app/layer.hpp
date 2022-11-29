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
    struct Layer
    {
        std::string name;

        struct Frame
        {
            Image* image;
            Texture* texture;
            bool is_tween_repeat = false;

            void draw_pixel(Vector2i, RGBA);
            void draw_image(Vector2i centroid, const Image&);
            void draw_line(Vector2i start, Vector2i end, RGBA);
            void draw_rectangle(Vector2i top_left, Vector2i bottom_right, size_t px, RGBA);
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
    void Layer::Frame::draw_pixel(Vector2i xy, RGBA color)
    {
        if (xy.x >= 0 and xy.x < image->get_size().x and xy.y >= 0 and xy.y < image->get_size().y)
            image->set_pixel(xy.x, xy.y, color);
    }

    void Layer::Frame::draw_image(Vector2i pos, const Image& image)
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
                    draw_pixel({x, y}, color);
            }
        }
    }

    void Layer::Frame::draw_line(Vector2i start, Vector2i end, RGBA color)
    {
        for (size_t x = start.x; x < end.x; ++x)
            for (size_t y = start.y; x < end.y; ++y)
                draw_pixel({x, y}, color);
    }

    void Layer::Frame::draw_rectangle(Vector2i top_left, Vector2i bottom_right, size_t px, RGBA color)
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

    void Layer::Frame::update_texture()
    {
        texture->create_from_image(*image);
    }
}
