//
// Copyright 2022 Clemens Cords
// Created on 8/6/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <string>
#include <mousetrap/gl_common.hpp>
#include <mousetrap/image.hpp>
#include <mousetrap/texture_object.hpp>
#include <mousetrap/wrap_mode.hpp>
#include <mousetrap/texture_scale_mode.hpp>
#include <mousetrap/signal_emitter.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    namespace detail
    {
        struct _TextureInternal
        {
            GObject parent;

            GLNativeHandle native_handle = 0;
            TextureWrapMode wrap_mode = TextureWrapMode::STRETCH;
            TextureScaleMode scale_mode = TextureScaleMode::NEAREST;
            Vector2i* size;
        };
        using TextureInternal = _TextureInternal;
    }
    #endif

    /// @brief texture object, image living GPU-side, immutable
    class Texture : public TextureObject, public SignalEmitter
    {
        public:
            /// @brief construct as texture of size 0x0
            Texture();

            /// @brief construct from already allocated GPU-side texture \for_internal_use_only
            Texture(GLNativeHandle);

            /// @brief construct from internal
            Texture(detail::TextureInternal*);

            /// @brief destruct, frees GPU-side memory
            ~Texture();

            /// @brief expose internal
            NativeObject get_internal() const override;

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
            void set_wrap_mode(TextureWrapMode);

            /// @brief get wrap mode, this governs how the texture behaves when the texture coordinates of a vertex are outside of [0, 1]
            /// @return wrap mode
            TextureWrapMode get_wrap_mode();

            /// @brief set scale mode, this governs how the texture behaves when displayed at a resolution other than it's native resolution
            /// @param scale_mode
            void set_scale_mode(TextureScaleMode scale_mode);

            /// @brief get scale mode, this governs how the texture behaves when displayed at a resolution other than it's native resolution
            /// @return scale mode
            TextureScaleMode get_scale_mode();

            /// @brief get native resolution of the texture
            /// @return width, height
            /// @note unlike in OpenGL, this operation is in O(1)
            Vector2i get_size() const;

            /// @brief get native OpenGL handle
            /// @return handle
            virtual GLNativeHandle get_native_handle() const;

            /// @brief expose as gobject, \for_internal_use_only
            operator GObject*() const override;

        private:
            detail::TextureInternal* _internal = nullptr;
    };
}
