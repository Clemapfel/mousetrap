// 
// Copyright 2022 Clemens Cords
// Created on 8/6/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/colors.hpp>
#include <gtk/gtk.h>
#include <vector>

namespace mousetrap
{
    class Image
    {
        public:
            Image() = default;

            Image(const Image&);
            Image(Image&&);
            Image& operator=(const Image&);
            Image& operator=(Image&&);

            void create(size_t width, size_t height, RGBA default_color = RGBA(0, 0, 0, 1));
            bool create_from_file(const std::string&);
            void create_from_pixbuf(GdkPixbuf*);
            void create_from_texture(GdkTexture*);

            bool save_to_file(const std::string&) const;

            void* data() const;
            /// \returns <number of pixels> * <number of components>
            size_t get_data_size() const;

            size_t get_n_pixels() const;

            GdkPixbuf* to_pixbuf() const;
            Vector2ui get_size() const;

            Image as_scaled(size_t size_x, size_t size_y, GdkInterpType type) const;
            Image as_cropped(int offset_x, int offset_y, size_t new_width, size_t new_height) const;
            Image as_flipped(bool flip_horizontally, bool flip_vertically) const;

            void set_pixel(size_t, size_t, RGBA);
            void set_pixel(size_t, size_t, HSVA);
            RGBA get_pixel(size_t, size_t) const;

            void set_pixel(size_t linear_index, RGBA);
            void set_pixel(size_t linear_index, HSVA);
            RGBA get_pixel(size_t linear_index) const;

        private:
            Vector2i _size;
            std::vector<float> _data; // rgba 32f per component

            size_t to_linear_index(size_t, size_t) const;
    };
}
