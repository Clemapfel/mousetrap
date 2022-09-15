// 
// Copyright 2022 Clemens Cords
// Created on 8/6/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <string>
#include <gtk/gtk.h>
#include <include/gl_common.hpp>
#include <include/image.hpp>
#include <include/texture_object.hpp>

namespace mousetrap
{
    enum class TextureWrapMode
    {
        ZERO = 0,
        ONE = 1,
        REPEAT = GL_REPEAT,
        MIRROR = GL_MIRRORED_REPEAT,
        STRETCH = GL_CLAMP_TO_EDGE
    };

    class Texture : public TextureObject
    {
        public:
            Texture(); // should be called while gl context is bound
            ~Texture();

            void bind(size_t texture_unit) const;

            void bind() const override;
            void unbind() const override;

            void create_from_file(const std::string& path);
            void create_from_image(const Image&);

            void set_wrap_mode(TextureWrapMode);
            TextureWrapMode get_wrap_mode();

            Vector2i get_size() const;

            GLNativeHandle get_native_handle() const;

        private:
            GLNativeHandle _native_handle = 0;
            TextureWrapMode _wrap_mode = TextureWrapMode::STRETCH;

            Vector2i _size;
    };
}

#include <src/texture.inl>