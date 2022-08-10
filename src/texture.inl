// 
// Copyright 2022 Clemens Cords
// Created on 8/6/22 by clem (mail@clemens-cords.com)
//

#include <iostream>

namespace mousetrap
{
    Texture::Texture()
    {
        glGenTextures(1, &_native_handle);
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &_native_handle);
    }

    void Texture::create_from_file(const std::string& path)
    {
        auto image = Image();
        image.create_from_file(path);

        create_from_image(image);
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
                     GL_FLOAT,
                     image.data()
        );

        _size = image.get_size();
    }

    void Texture::bind(size_t texture_unit)
    {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D, _native_handle);

        if (_wrap_mode == ZERO)
        {
            static float zero_border[] = {0.f, 0.f, 0.f, 0.f};
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, zero_border);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        }
        else if (_wrap_mode == ONE)
        {
            static float one_border[] = {0.f, 0.f, 0.f, 0.f};
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, one_border);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _wrap_mode);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _wrap_mode);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    void Texture::unbind()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::set_wrap_mode(TextureWrapMode wrap_mode)
    {
        _wrap_mode = wrap_mode;
    }

    TextureWrapMode Texture::get_wrap_mode()
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
}