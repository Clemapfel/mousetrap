// 
// Copyright 2022 Clemens Cords
// Created on 9/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

namespace mousetrap
{
    struct Layer
    {
        std::string name;

        struct Frame
        {
            Image* image;
            Texture* texture;
            bool is_keyframe = true;

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
