// 
// Copyright 2022 Clemens Cords
// Created on 8/6/22 by clem (mail@clemens-cords.com)
//

#include <iostream>

namespace mousetrap
{
    inline Texture::Texture()
    {
        glGenTextures(1, &_native_handle);
    }

    inline Texture::~Texture()
    {
        if (_native_handle != 0)
            glDeleteTextures(1, &_native_handle);
    }

    inline void Texture::create(size_t width, size_t height)
    {
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, _native_handle);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA32F,
                     width,
                     height,
                     0,
                     GL_RGBA,
                     GL_FLOAT,
                     nullptr
        );

        _size = {width, height};
    }

    inline void Texture::create_from_file(const std::string& path)
    {
        auto image = Image();
        image.create_from_file(path);

        create_from_image(image);
    }

    inline Texture::Texture(Texture&& other)
    {
        _native_handle = other._native_handle;
        _size = other._size;
        _wrap_mode = other._wrap_mode;

        other._native_handle = 0;
        other._size = {0, 0};
    }

    inline Texture& Texture::operator=(Texture&& other)
    {
        _native_handle = other._native_handle;
        _size = other._size;
        _wrap_mode = other._wrap_mode;

        other._native_handle = 0;
        other._size = {0, 0};

        return *this;
    }

    inline void Texture::create_from_image(const Image& image)
    {
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, _native_handle);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA32F,
                     image.get_size().x,
                     image.get_size().y,
                     0,
                     GL_RGBA,
                     GL_FLOAT,
                     image.data()
        );

        _size = image.get_size();
    }

    inline void Texture::bind(size_t texture_unit) const
    {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D, _native_handle);

        if (_wrap_mode == TextureWrapMode::ZERO)
        {
            static float zero_border[] = {0.f, 0.f, 0.f, 0.f};
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, zero_border);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        }
        else if (_wrap_mode == TextureWrapMode::ONE)
        {
            static float one_border[] = {0.f, 0.f, 0.f, 0.f};
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, one_border);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint) _wrap_mode);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint) _wrap_mode);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    inline void Texture::bind() const
    {
        bind(0);
    }

    inline void Texture::unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    inline void Texture::set_wrap_mode(TextureWrapMode wrap_mode)
    {
        _wrap_mode = wrap_mode;
    }

    inline TextureWrapMode Texture::get_wrap_mode()
    {
        return _wrap_mode;
    }

    inline Vector2i Texture::get_size() const
    {
        return _size;
    }

    inline GLNativeHandle Texture::get_native_handle() const
    {
        return _native_handle;
    }
}