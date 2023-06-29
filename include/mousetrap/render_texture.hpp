//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 2/6/23
//

#pragma once

#include <mousetrap/gl_common.hpp>
#if MOUSETRAP_ENABLE_OPENGL_COMPONENT

#include <mousetrap/texture.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class RenderTexture;
    namespace detail
    {
        struct _RenderTextureInternal
        {
            GObject parent;
            GLNativeHandle framebuffer_handle;
            GLint before_buffer;
        };
        using RenderTextureInternal = _RenderTextureInternal;
        DEFINE_INTERNAL_MAPPING(RenderTexture);
    }
    #endif

    /// @brief texture that can be bound, such that any rendering happening afterwards will be pushed into the textures framebuffer. It can still be used like a regular texture
    class RenderTexture : public Texture
    {
        public:
            /// @brief construct as empty texture of size 0x0
            RenderTexture();

            /// @brief construct from internal
            RenderTexture(detail::RenderTextureInternal*);

            /// @brief destructor
            ~RenderTexture();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief get framebuffer id
            GLNativeHandle get_native_handle() const override;

            /// @brief copy ctor deleted
            RenderTexture(const RenderTexture&) = delete;

            /// @brief copy assignment deleted
            RenderTexture& operator=(const RenderTexture&) = delete;

            /// @brief move ctor, safely transfers ownership of GPU-side objects
            /// @param other
            RenderTexture(RenderTexture&&);

            /// @brief move assignment, safely transfers ownership of GPU-side objects
            /// @param other
            /// @returns reference to self after assignment
            RenderTexture& operator=(RenderTexture&&);

            /// @brief bind texture as render target, from this point on all render calls will write to its internal framebuffer instead
            void bind_as_render_target() const;

            /// @brief unbind as render target, restores the framebuffer that was active before mousetrap::RenderTexture::bind_as_rendertarget was called
            void unbind_as_render_target() const;

            /// @brief expose as gobject
            operator GObject*() const override;

        private:
            detail::RenderTextureInternal* _internal = nullptr;
    };
}

#endif // MOUSETRAP_ENABLE_OPENGL_COMPONENT

