//
// Copyright 2022 Clemens Cords
// Created on 8/6/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/color.hpp>
#include <include/vector.hpp>
#include <include/scale_mode.hpp>

#include <gtk/gtk.h>
#include <vector>

namespace mousetrap
{
    enum class InterpolationType
    {
        NEAREST = GDK_INTERP_NEAREST,
        TILES = GDK_INTERP_TILES,
        BILINEAR = GDK_INTERP_BILINEAR,
        HYPERBOLIC = GDK_INTERP_HYPER
    };

    class Image
    {
        public:
            Image() = default;
            ~Image();

            Image(const Image&);
            Image(Image&&) noexcept;
            Image& operator=(const Image&);
            Image& operator=(Image&&) noexcept;

            void create(size_t width, size_t height, RGBA default_color = RGBA(0, 0, 0, 1));
            bool create_from_file(const std::string&);

            bool save_to_file(const std::string&) const;

            void* data() const;
            size_t get_data_size() const;
            size_t get_n_pixels() const;
            Vector2ui get_size() const;

            Image as_scaled(size_t size_x, size_t size_y, InterpolationType) const;
            Image as_cropped(int offset_x, int offset_y, size_t new_width, size_t new_height) const;
            Image as_flipped(bool flip_horizontally, bool flip_vertically) const;

            void set_pixel(size_t, size_t, RGBA);
            void set_pixel(size_t, size_t, HSVA);
            RGBA get_pixel(size_t, size_t) const;

            void set_pixel(size_t linear_index, RGBA);
            void set_pixel(size_t linear_index, HSVA);
            RGBA get_pixel(size_t linear_index) const;

        private:
            Image(GdkPixbuf* pixbuf);

            Vector2i _size = {0, 0};
            GdkPixbuf* _data = nullptr;

            size_t to_linear_index(size_t, size_t) const;
    };
}
