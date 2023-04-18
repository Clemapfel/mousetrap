//
// Copyright 2022 Clemens Cords
// Created on 8/6/22 by clem (mail@clemens-cords.com)
//

#include <include/image.hpp>
#include <include/log.hpp>

#include <sstream>
#include <iostream>

namespace mousetrap
{
    Image::~Image()
    {
        g_object_unref(_data);
    }

    Image::Image(GdkPixbuf* pixbuf)
        : _data(pixbuf)
    {
        g_object_ref(_data);
        _size.x = gdk_pixbuf_get_width(pixbuf);
        _size.y = gdk_pixbuf_get_height(pixbuf);
    }

    Image::Image(const Image& other)
    {
        _data = gdk_pixbuf_copy(other._data);
        _size = other._size;
    }

    Image::Image(Image&& other) noexcept
    {
        g_object_unref(_data);
        g_object_ref(other._data);

        _data = other._data;
        _size = other._size;

        other._data = nullptr;
        other._size = {0, 0};
    }

    Image& Image::operator=(const Image& other)
    {
        _data = gdk_pixbuf_copy(other._data);
        _size = other._size;
        return *this;
    }

    Image& Image::operator=(Image&& other) noexcept
    {
        g_object_unref(_data);
        g_object_ref(other._data);

        _data = other._data;
        _size = other._size;

        other._data = nullptr;
        other._size = {0, 0};
        return *this;
    }

    Image::operator GdkPixbuf*() const
    {
        return _data;
    }

    void Image::create(size_t width, size_t height, RGBA default_color)
    {
        _data = gdk_pixbuf_new(GDK_COLORSPACE_RGB, GL_TRUE, 8, width, height);
        _size = {width, height};

        g_object_ref(_data);
    }

    bool Image::create_from_file(const std::string& path)
    {
        GError* error_maybe = nullptr;
        _data = gdk_pixbuf_new_from_file(path.c_str(), &error_maybe);

        if (error_maybe != nullptr)
        {
            log::critical("In Image::create_from_file: unable to open file \"" + path + "\"", MOUSETRAP_DOMAIN);
            g_error_free(error_maybe);
            _size = {0, 0};
            return false;
        }

        _size.x = gdk_pixbuf_get_width(_data);
        _size.y = gdk_pixbuf_get_height(_data);

        g_object_ref(_data);
        return true;
    }

    bool Image::save_to_file(const std::string& path) const
    {
        if (_size.x == 0 and _size.y == 0)
        {
            std::cerr << "[WARNING] In Image::save_to_file: Attempting to write an image of size 0x0 to disk, no file will be generated." << std::endl;
            return false;
        }

        GError* error = nullptr;
        gdk_pixbuf_save(_data, path.c_str(), "png", &error, NULL);
        if (error != nullptr)
        {
            log::critical(std::string("In Image::save_to_file: ") + error->message, MOUSETRAP_DOMAIN);
            g_error_free(error);
            return false;
        }

        return true;
    }

    Vector2ui Image::get_size() const
    {
        return _size;
    }

    void* Image::data() const
    {
        return gdk_pixbuf_get_pixels(_data);
    }

    size_t Image::get_data_size() const
    {
        return _size.x * _size.y * 4;
    }

    size_t Image::get_n_pixels() const
    {
        return get_size().x * get_size().y;
    }

    size_t Image::to_linear_index(size_t x, size_t y) const
    {
        return y * (_size.x * 4) + (x * 4);
    }

    void Image::set_pixel(size_t x, size_t y, RGBA color)
    {
        auto i = to_linear_index(x, y);

        if (i >= get_data_size())
        {
            std::cerr << "[ERROR] In Image::set_pixel: indices " << x << " " << y << " are out of bounds for an image of size " << _size.x << "x" << _size.y << std::endl;
            return;
        }

        auto* data = gdk_pixbuf_get_pixels(_data);
        data[i] = color.r * 255;
        data[i+1] = color.g * 255;
        data[i+2] = color.b * 255;
        data[i+3] = color.a * 255;
    }

    void Image::set_pixel(size_t x, size_t y, HSVA color)
    {
        set_pixel(x, y, color.operator RGBA());
    }

    RGBA Image::get_pixel(size_t x, size_t y) const
    {
        auto i = to_linear_index(x, y);

        if (i >= get_data_size())
        {
            std::stringstream str;
            str << "[ERROR] In Image::get_pixel: indices " << x << " " << y << " are out of bounds for an image of size " << _size.x << "x" << _size.y;
            log::critical(str.str(), MOUSETRAP_DOMAIN);

            return RGBA(0, 0, 0, 0);
        }

        auto* data = gdk_pixbuf_get_pixels(_data);

        return RGBA (
        data[i] / 255.f,
        data[i+1] / 255.f,
        data[i+2] / 255.f,
        data[i+3] / 255.f
        );
    }

    void Image::set_pixel(size_t i, RGBA color)
    {
        i *= 4;

        if (i >= get_data_size())
        {
            std::stringstream str;
            str << "In Image::set_pixel: index " << i / 4 << " out of bounds for an image of with " << _size.x * _size.y << " pixels";

            log::critical(str.str(), MOUSETRAP_DOMAIN);
            return;
        }

        auto* data = gdk_pixbuf_get_pixels(_data);
        data[i] = color.r * 255;
        data[i+1] = color.g * 255;
        data[i+2] = color.b * 255;
        data[i+3] = color.a * 255;
    }

    void Image::set_pixel(size_t i, HSVA color_hsva)
    {
        set_pixel(i, color_hsva.operator RGBA());
    }

    RGBA Image::get_pixel(size_t i) const
    {
        i *= 4;

        auto* data = gdk_pixbuf_get_pixels(_data);

        return RGBA (
            data[i] / 255.f,
            data[i+1] / 255.f,
            data[i+2] / 255.f,
            data[i+3] / 255.f
        );
    }

    Image Image::as_cropped(int offset_x, int offset_y, size_t size_x, size_t size_y) const
    {
        auto out = Image();
        out.create(size_x, size_y);

        for (size_t y = 0; y < size_y; ++y)
        {
            for (size_t x = 0; x < size_x; ++x)
            {
                Vector2i pos = {x - offset_x, y - offset_y};

                if (pos.x < 0 or pos.x >= get_size().x or pos.y < 0 or pos.y >= get_size().y)
                    out.set_pixel(x, y, RGBA(0, 0, 0, 0));
                else
                    out.set_pixel(x, y, get_pixel(pos.x, pos.y));
            }
        }

        return out;
    }

    Image Image::as_scaled(size_t size_x, size_t size_y, InterpolationType type) const
    {
        if (int(size_x) == _size.x and int(size_y) == _size.y)
            return *this;

        if (size_x == size_t(0))
            size_x = 1;

        if (size_y == size_t(0))
            size_y = 1;

        GdkInterpType gdk_interpolation_type;
        GdkPixbuf* unscaled = _data;
        return Image(gdk_pixbuf_scale_simple(unscaled, size_x, size_y, (GdkInterpType) type));
    }

    Image Image::as_flipped(bool flip_horizontally, bool flip_vertically) const
    {
        auto out = Image();
        out.create(_size.x, _size.y);

        for (size_t x = 0; x < _size.x; ++x)
        {
            for (size_t y = 0; y < _size.y; ++y)
            {
                Vector2ui pos = {x, y};

                if (flip_horizontally)
                    pos.x = _size.x - pos.x - 1;

                if (flip_vertically)
                    pos.y = _size.y - pos.y - 1;

                out.set_pixel(pos.x, pos.y, get_pixel(x, y));
            }
        }

        return out;
    }
}