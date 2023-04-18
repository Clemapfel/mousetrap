//
// Copyright 2022 Clemens Cords
// Created on 8/6/22 by clem (mail@clemens-cords.com)
//

#include <iostream>
#include <include/texture.hpp>

namespace mousetrap
{
    Texture::Texture()
    {
        glGenTextures(1, &_native_handle);
    }

    Texture::Texture(GLNativeHandle handle)
        : _native_handle(handle)
    {
        glBindTexture(GL_TEXTURE_2D, handle);

        int width = 0;
        int height = 0;

        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
        _size = {width, height};
    }

    Texture::~Texture()
    {
        if (_native_handle != 0)
            glDeleteTextures(1, &_native_handle);
    }

    void Texture::create(size_t width, size_t height)
    {
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, _native_handle);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glTexImage2D(GL_TEXTURE_2D,
             0,
             GL_RGBA16F,
             width,
             height,
             0,
             GL_RGBA,
             GL_UNSIGNED_BYTE,
             nullptr
        );

        _size = {width, height};
    }

    void Texture::create_from_file(const std::string& path)
    {
        auto image = Image();
        image.create_from_file(path);

        create_from_image(image);
    }

    Texture::Texture(Texture&& other) noexcept
    {
        _native_handle = other._native_handle;
        _size = other._size;
        _wrap_mode = other._wrap_mode;

        other._native_handle = 0;
        other._size = {0, 0};
    }

    Texture& Texture::operator=(Texture&& other) noexcept
    {
        _native_handle = other._native_handle;
        _size = other._size;
        _wrap_mode = other._wrap_mode;

        other._native_handle = 0;
        other._size = {0, 0};

        return *this;
    }

    void Texture::create_from_image(const Image& image)
    {
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, _native_handle);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glTexImage2D(GL_TEXTURE_2D,
             0,
             GL_RGBA16F,
             image.get_size().x,
             image.get_size().y,
             0,
             GL_RGBA,
             GL_UNSIGNED_BYTE,
             image.data()
        );

        _size = image.get_size();
    }

    void Texture::bind(size_t texture_unit) const
    {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D, _native_handle);

        if (_wrap_mode == WrapMode::ZERO)
        {
            static float zero_border[] = {0.f, 0.f, 0.f, 0.f};
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, zero_border);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        }
        else if (_wrap_mode == WrapMode::ONE)
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


        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint) _scale_mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint) _scale_mode);
    }

    void Texture::bind() const
    {
        bind(0);
    }

    void Texture::unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::set_wrap_mode(WrapMode wrap_mode)
    {
        _wrap_mode = wrap_mode;
    }

    WrapMode Texture::get_wrap_mode()
    {
        return _wrap_mode;
    }

    Vector2i Texture::get_size() const
    {
        return _size;
    }

    GLNativeHandle Texture::get_native_handle() const
    {
        return _native_handle;
    }

    void Texture::set_scale_mode(TextureScaleMode mode)
    {
        _scale_mode = mode;
    }

    TextureScaleMode Texture::get_scale_mode()
    {
        return _scale_mode;
    }

    Image Texture::download() const
    {
        auto out = Image();
        out.create(_size.x, _size.y);

        glBindTexture(GL_TEXTURE_2D, _native_handle);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, out.data());
        glBindTexture(GL_TEXTURE_2D, 0);

        return out;
    }
}