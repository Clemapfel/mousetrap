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
            void create(size_t width, size_t height, RGBA default_color = RGBA(0, 0, 0, 1));
            void create_from_file(const std::string&);
            void create_from_pixbuf(GdkPixbuf*);

            void* data() const;
            size_t get_data_size() const;

            GdkPixbuf* to_pixbuf() const;

            Vector2i get_size() const;

            void set_pixel(size_t, size_t, RGBA);
            void set_pixel(size_t, size_t, HSVA);
            RGBA get_pixel(size_t, size_t);

            void set_pixel(size_t linear_index, RGBA);
            void set_pixel(size_t linear_index, HSVA);
            RGBA get_pixel(size_t linear_index);

        private:
            Vector2i _size;
            std::vector<float> _data; // rgba 32f bpc

            size_t to_linear_index(size_t, size_t);
    };
}

#include <src/image.inl>