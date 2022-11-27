// 
// Copyright 2022 Clemens Cords
// Created on 8/6/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    void Image::create(size_t width, size_t height, RGBA default_color)
    {
        _data.clear();
        _data.reserve(width * height * 4);
        for (size_t i = 0; i < width * height; ++i)
        {
            _data.push_back(default_color.r);
            _data.push_back(default_color.g);
            _data.push_back(default_color.b);
            _data.push_back(default_color.a);
        }

        _size = {width, height};
    }

    void Image::create_from_pixbuf(GdkPixbuf* pixbuf)
    {
        unsigned char* buffer = gdk_pixbuf_get_pixels(pixbuf);

        bool has_alpha = gdk_pixbuf_get_has_alpha(pixbuf);
        size_t padding_bytes = gdk_pixbuf_get_rowstride(pixbuf) / sizeof(unsigned char) - gdk_pixbuf_get_width(pixbuf) * (has_alpha ? 4 : 3);
        _size = {gdk_pixbuf_get_width(pixbuf) - padding_bytes, gdk_pixbuf_get_height(pixbuf)};
        size_t n = _size.x * _size.y * (has_alpha ? 4 : 3);

        _data.clear();
        _data.reserve(n);

        for (size_t i = 0; i < n; ++i)
        {
            _data.push_back(buffer[i] / 255.f);

            if (not has_alpha)
                _data.push_back(1);
        }
    }

    bool Image::create_from_file(const std::string& path)
    {
        GError* error_maybe = nullptr;
        auto* pixbuf = gdk_pixbuf_new_from_file(path.c_str(), &error_maybe);

        if (error_maybe != nullptr)
        {
            std::cerr << "[WARNING] In Image::create_from_file: unable to open file \"" << path << "\"" << std::endl;
            _data.clear();
            _size = {0, 0};
            return false;
        }

        create_from_pixbuf(pixbuf);
        g_object_unref(pixbuf);
        return true;
    }

    GdkPixbuf* Image::to_pixbuf() const
    {
        auto* out = gdk_pixbuf_new(GDK_COLORSPACE_RGB, true, 8, _size.x, _size.y);
        auto* data = gdk_pixbuf_get_pixels(out);

        for (size_t i = 0; i < _data.size(); ++i)
            data[i] = uint8_t(_data.at(i) * 255.f);

        return out;
    }

    Vector2ui Image::get_size() const
    {
        return _size;
    }

    void* Image::data() const
    {
        return (void*) _data.data();
    }

    size_t Image::get_data_size() const
    {
        return _data.size();
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

        if (i >= _data.size())
        {
            std::cerr << "[ERROR] In Image::set_pixel: indices " << x << " " << y << " are out of bounds for an image of size " << _size.x << "x" << _size.y << std::endl;
            return;
        }

        _data.at(i) = color.r;
        _data.at(i+1) = color.g;
        _data.at(i+2) = color.b;
        _data.at(i+3) = color.a;
    }

    void Image::set_pixel(size_t x, size_t y, HSVA color)
    {
        set_pixel(x, y, color.operator RGBA());
    }

    RGBA Image::get_pixel(size_t x, size_t y) const
    {
        auto i = to_linear_index(x, y);

        if (i >= _data.size())
        {
            std::cerr << "[ERROR] In Image::get_pixel: indices " << x << " " << y << " are out of bounds for an image of size " << _size.x << "x" << _size.y << std::endl;
            return RGBA(0, 0, 0, 0);
        }

        return RGBA
        (
                _data.at(i),
                _data.at(i+1),
                _data.at(i+2),
                _data.at(i+3)
        );
    }

    void Image::set_pixel(size_t i, RGBA color)
    {
        i *= 4;

        _data.at(i) = color.r;
        _data.at(i+1) = color.g;
        _data.at(i+2) = color.b;
        _data.at(i+3) = color.a;
    }

    void Image::set_pixel(size_t i, HSVA color_hsva)
    {
        auto color = color_hsva.operator RGBA();

        i *= 4;

        _data.at(i) = color.r;
        _data.at(i+1) = color.g;
        _data.at(i+2) = color.b;
        _data.at(i+3) = color.a;
    }

    RGBA Image::get_pixel(size_t i) const
    {
        return RGBA
        (
            _data.at(i),
            _data.at(i+1),
            _data.at(i+2),
            _data.at(i+3)
        );
    }

    Image Image::as_cropped(size_t x_min, size_t y_min, size_t x_max, size_t y_max)
    {
        size_t w = get_size().x;
        size_t h = get_size().y;

        if (x_min > x_max)
            x_min = x_max;

        if (y_min > y_max)
            y_min = y_max;

        auto out = Image();
        out.create(x_max - x_min, y_max - y_min);

        for (size_t x = x_min; x < x_max; ++x)
            for (size_t y = y_min; y < y_max; ++y)
                out.set_pixel(x, y, get_pixel(x, y));

        return out;
    }

    Image Image::as_scaled(size_t size_x, size_t size_y)
    {
        if (size_x == _size.x and size_y == _size.y)
            return *this;

        if (size_x == 0)
            size_x = 1;

        if (size_y == 0)
            size_y = 1;

        GdkPixbuf* unscaled = g_object_ref(to_pixbuf());
        auto scaled = g_object_ref(gdk_pixbuf_scale_simple(unscaled, size_x, size_y, GDK_INTERP_NEAREST));

        auto out = Image();
        out.create_from_pixbuf(scaled);

        g_object_unref(unscaled);
        g_object_unref(scaled);

        return out;
    }
}