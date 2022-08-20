// 
// Copyright 2022 Clemens Cords
// Created on 8/15/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    PixelBuffer::PixelBuffer(size_t width, size_t height)
            : _size(width, height)
    {
        glGenBuffers(1, &_native_handle);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _native_handle);
        glBufferData(GL_PIXEL_UNPACK_BUFFER,
                     width * height * 4 * sizeof(float),
                     nullptr,
                     GL_DYNAMIC_DRAW
        );

        void* cache_maybe = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);

        if (cache_maybe == nullptr)
            std::cerr << "[ERROR] In PixelBuffer::PixelBuffer: glMapBuffer returned NULL" << std::endl;

        _cache = (float*) cache_maybe;

        for (size_t i = 0; i < width * height * 4; ++i)
            _cache[i] = 0;

        unbind();
    }

    PixelBuffer::~PixelBuffer()
    {
        glDeleteBuffers(1, &_native_handle);
    }

    float* PixelBuffer::get_data()
    {
        return _cache;
    }

    void PixelBuffer::bind() const
    {
        if (_bound)
            return;

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _native_handle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _size.x, _size.y, 0, GL_RGBA, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        static float zero_border[] = {0.f, 0.f, 0.f, 0.f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, zero_border);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        _bound = true;
    }

    void PixelBuffer::unbind() const
    {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        _bound = false;
    }

    void PixelBuffer::flush() const
    {
        bind();
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
        unbind();
    }

    Vector2ui PixelBuffer::get_size() const
    {
        return _size;
    }

    size_t PixelBuffer::to_linear_index(size_t x, size_t y)
    {
        return y * (_size.x * 4) + (x * 4);
    }

    void PixelBuffer::set_pixel(size_t x, size_t y, RGBA color)
    {
        auto i = to_linear_index(x, y);
        if (i >= _size.x * _size.y * 4)
        {
            std::cerr << "[ERROR] In Image::set_pixel: indices " << x << " " << y << " are out of bounds for an image of size " << _size.x << "x" << _size.y << std::endl;
            return;
        }

        _cache[i] = color.r;
        _cache[i+1] = color.g;
        _cache[i+2] = color.b;
        _cache[i+3] = color.a;
    }

    void PixelBuffer::set_pixel(size_t i, RGBA color)
    {
        if (i >= _size.x * _size.y * 4)
        {
            std::cerr << "[ERROR] In Image::set_pixel: index " << i << " are out of bounds for an image of size " << _size.x << "x" << _size.y << std::endl;
            return;
        }

        _cache[i] = color.r;
        _cache[i+1] = color.g;
        _cache[i+2] = color.b;
        _cache[i+3] = color.a;
    }

    RGBA PixelBuffer::get_pixel(size_t x, size_t y)
    {
        auto i = to_linear_index(x, y);

        if (i >= _size.x * _size.y * 4)
        {
            std::cerr << "[ERROR] In Image::set_pixel: indices " << x << " " << y << " are out of bounds for an image of size " << _size.x << "x" << _size.y << std::endl;
            return RGBA(0, 0, 0, 0);
        }

        return RGBA(
                _cache[i],
                _cache[i+1],
                _cache[i+2],
                _cache[i+3]
        );
    }
}