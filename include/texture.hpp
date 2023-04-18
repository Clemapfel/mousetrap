//
// Copyright 2022 Clemens Cords
// Created on 8/6/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <string>
#include <include/gl_common.hpp>
#include <include/image.hpp>
#include <include/texture_object.hpp>
#include <include/wrap_mode.hpp>
#include <include/scale_mode.hpp>

namespace mousetrap
{
    /// @brief texture object, image living GPU-side, immutable
    class Texture : public TextureObject
    {
        public:
            /// @brief construct as texture of size 0x0
            Texture();

            /// @brief construct from already allocated GPU-side texture \internal
            Texture(GLNativeHandle);

            /// @brief destruct, frees GPU-side memory
            virtual ~Texture();

            /// @brief copy ctor deleted
            Texture(const Texture&) = delete;

            /// @brief copy assignment deleted
            Texture& operator=(const Texture&) = delete;

            /// @brief move ctor, safely transfers ownership without allocating a new GPU-side texture object
            /// @param other
            Texture(Texture&&) noexcept;

            /// @brief move assignment, safely transfers ownership without allocating a new GPU-side texture object
            /// @param other
            /// @return reference to self after assignment
            Texture& operator=(Texture&&) noexcept;

            /// @brief download texture data into a CPU-side image, this is an extremely costly operation
            [[nodiscard]] Image download() const;

            /// @brief bind the texture for rendering
            /// @param texture_unit texture unit to bind to, usually <tt>GL_TEXTURE0 + n</tt> where n = 0, 1, ...
            void bind(size_t texture_unit) const;

            /// @brief bind the texture for rendering, bound to GL_TEXTURE0
            void bind() const override;

            /// @brief unbind texture
            void unbind() const override;

            /// @brief create texture as an image of given size with all pixels set to RGBA(0, 0, 0, 0)
            /// @param width
            /// @param height
            void create(size_t width, size_t height);

            /// @brief create texture from an image on disk
            /// @param path absolute path
            void create_from_file(const std::string& path);

            /// @brief create from image
            /// @param image
            void create_from_image(const Image&);

            /// @brief set wrap mode, this governs how the texture behaves when the texture coordinates of a vertex are outside of [0, 1]
            /// @param wrap_mode
            void set_wrap_mode(WrapMode);

            /// @brief get wrap mode, this governs how the texture behaves when the texture coordinates of a vertex are outside of [0, 1]
            /// @return wrap mode
            WrapMode get_wrap_mode();

            /// @brief set scale mode, this governs how the texture behaves when displayed at a resolution other than it's native resolution
            /// @param scale_mode
            void set_scale_mode(TextureScaleMode);

            /// @brief get scale mode, this governs how the texture behaves when displayed at a resolution other than it's native resolution
            /// @return scale mode
            TextureScaleMode get_scale_mode();

            /// @brief get native resolution of the texture
            /// @return width, height
            /// @note unlike in OpenGL, this operation is in O(1)
            Vector2i get_size() const;

            /// @brief get native OpenGL handle
            /// @return handle
            GLNativeHandle get_native_handle() const;

        private:
            GLNativeHandle _native_handle = 0;
            WrapMode _wrap_mode = WrapMode::STRETCH;
            TextureScaleMode _scale_mode = TextureScaleMode::NEAREST;

            Vector2i _size;
    };
}
