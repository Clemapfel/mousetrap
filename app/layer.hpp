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

    void Layer::Frame::update_texture()
    {
        texture->create_from_image(*image);
    }
}
