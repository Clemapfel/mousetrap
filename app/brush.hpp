// 
// Copyright 2022 Clemens Cords
// Created on 11/25/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

#include <app/config_files.hpp>
#include <app/algorithms.hpp>

namespace mousetrap
{
    enum class BrushShape
    {
        CIRCLE,
        SQUARE,
        ELLIPSE_HORIZONTAL,
        ELLIPSE_VERTICAL,
        RECTANGLE_HORIZONTAL,
        RECTANGLE_VERTICAL,
        CUSTOM
    };

    std::string brush_shape_to_string(BrushShape);
    BrushShape brush_shape_from_string(const std::string&);

    class Brush
    {
        public:
            Brush() = default;
            ~Brush();

            void as_circle();
            void as_square();
            void as_ellipse_horizontal();
            void as_ellipse_vertical();
            void as_rectangle_horizontal();
            void as_rectangle_vertical();
            void as_custom(Image& base_image);

            void set_size(size_t);
            size_t get_size() const;
            const Image& get_image() const;
            BrushShape get_shape() const;
            std::string get_name() const;

        private:
            Image _image;

            void update_image();
            size_t _size = 1;
            std::string _name;

            // for custom brush mode
            Image _base_image;
            BrushShape _shape;
    };
}