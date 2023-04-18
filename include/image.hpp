//
// Copyright 2022 Clemens Cords
// Created on 8/6/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gtk_common.hpp>
#include <include/color.hpp>
#include <include/vector.hpp>
#include <include/scale_mode.hpp>

#include <vector>

namespace mousetrap
{
    /// @brief image scale interpolation type
    /// @see https://docs.gtk.org/gdk-pixbuf/enum.InterpType.html
    enum class InterpolationType
    {
        /// @brief nearest neighbor scaling, fastest
        NEAREST = GDK_INTERP_NEAREST,

        /// @brief nearest neighbor for enlargement, bilinear for shrinking
        TILES = GDK_INTERP_TILES,

        /// @brief bilinear interpolation, average speed
        BILINEAR = GDK_INTERP_BILINEAR,

        /// @brief hyperbolic interpolation, slowest
        HYPERBOLIC = GDK_INTERP_HYPER
    };

    /// @brief a 2d buffer container RGBA pixels
    class Image
    {
        public:
            /// @brief default ctor, initialize as image of size 0x0
            Image() = default;

            /// @brief destruct, frees data
            ~Image();

            /// @brief copy ctor, invokes deep copy of the data
            /// @param other
            Image(const Image&);

            /// @brief move ctor, moves data, empties original image
            /// @param original
            Image(Image&&) noexcept;

            /// @brief copy assignment, invokes deep copy of the data
            /// @param other
            /// @return self after assignment
            Image& operator=(const Image&);

            /// @brief move assignment, empties original image
            /// @param other
            /// @return self after assignment
            Image& operator=(Image&&) noexcept;

            /// @brief expose as GdkPixbuf, for interal use only
            operator GdkPixbuf*() const;

            /// @brief create image of given size and identical color
            /// @param width new x-dimension
            /// @param height new y-dimension
            /// @param default_color color of all pixels after initialization
            void create(size_t width, size_t height, RGBA default_color = RGBA(0, 0, 0, 1));

            /// @brief create an image by reading a file
            /// @param path
            /// @return true if successfull, false otherwise
            bool create_from_file(const std::string&);

            /// @brief sae to file
            /// @param path
            /// @return true if successfull, false otherwise
            bool save_to_file(const std::string&) const;

            /// @brief expose pixel data, linear array of RGBA values. For internal use only
            /// @return void pointer to data
            void* data() const;

            /// @brief get length of linear data
            /// @return n
            size_t get_data_size() const;

            /// @brief get number of pixels
            /// @return n, equal to width * height
            size_t get_n_pixels() const;

            /// @brief get dimensions
            /// @return size
            Vector2ui get_size() const;

            /// @brief create a new image, scaled according to the scale mode
            /// @param size_x new width
            /// @param size_y new height
            /// @param interpolation_type interpolation algorithm to use
            /// @return newly allocated image
            [[nodiscard]] Image as_scaled(size_t size_x, size_t size_y, InterpolationType) const;

            /// @brief create a new image, cropped
            /// @param offset_x left most anchor of the newly cropped image, may be negative
            /// @param offset_y top most anchor of the newly cropped image, may be negative
            /// @param new_width new width
            /// @param new_height new height
            /// @return newly allocated image
            /// @note pixels outside the original image bounds will be set to RBGA(0, 0, 0, 0)
            [[nodiscard]] Image as_cropped(int offset_x, int offset_y, size_t new_width, size_t new_height) const;

            /// @brief create a new image, flipped
            /// @param flip_horizontally flip along the x-axis
            /// @param flip_vertically flip along the y-axis
            /// @returns newly allocated image, same size as original image
            [[nodiscard]] Image as_flipped(bool flip_horizontally, bool flip_vertically) const;

            /// @brief set value of individual pixel, prints soft warning if indices out of bounds
            /// @param x row-index
            /// @param y column-index
            /// @param new_color RGBA
            void set_pixel(size_t, size_t, RGBA);

            /// @brief set value of individual pixel, prints soft warning if indices out of bounds
            /// @param x row-index
            /// @param y column-index
            /// @param new_color HSVA
            void set_pixel(size_t, size_t, HSVA);

            /// @brief get value of individual pixel, prints soft warning if indices out of bounds
            /// @param x row-index
            /// @param y column-index
            /// @returns pixel if indices in bounds, RGBA(0, 0, 0, 0) otherwise
            RGBA get_pixel(size_t, size_t) const;

            /// @brief set value of individual pixel, linear indexing. Prints soft warning if index out of bounds
            /// @param linear_index
            /// @param new_color RBGA
            void set_pixel(size_t linear_index, RGBA);

            /// @brief set value of individual pixel, linear indexing. Prints soft warning if index out of bounds
            /// @param linear_index
            /// @param new_color HSVA
            void set_pixel(size_t linear_index, HSVA);

            /// @brief get value of individual pixel, linear indexing.
            /// @param linear_index
            /// @return RGBA
            RGBA get_pixel(size_t linear_index) const;

        private:
            Image(GdkPixbuf* pixbuf);

            Vector2i _size = {0, 0};
            GdkPixbuf* _data = nullptr;

            size_t to_linear_index(size_t, size_t) const;
    };
}
