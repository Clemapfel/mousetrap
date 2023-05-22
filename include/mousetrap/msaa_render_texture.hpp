//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 2/15/23
//

#pragma once

#include <mousetrap/gl_common.hpp>
#include <mousetrap/texture_object.hpp>
#include <mousetrap/signal_emitter.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class MultisampledRenderTexture;
    namespace detail
    {
        struct _MultisampledRenderTextureInternal
        {
            GObject parent;

            size_t n_samples;

            size_t width = 0;
            size_t height = 0;

            mutable GLint before_buffer = 0;

            GLNativeHandle buffer = 0;
            GLNativeHandle msaa_color_buffer_texture = 0;
            GLNativeHandle intermediate_buffer = 0;
            GLNativeHandle screen_texture = 0;
        };
        using MultisampledRenderTextureInternal = _MultisampledRenderTextureInternal;
        DEFINE_INTERNAL_MAPPING(MultisampledRenderTexture);
    }
    #endif

    /// @brief render texture that utilizes a multi-sampled buffer, which renders the resulting image anti-aliased
    class MultisampledRenderTexture : public TextureObject, public SignalEmitter
    {
        public:
            /// @brief construct
            /// @param n_samples number of MSAA samples, usually 2, 4, 8 or 16
            MultisampledRenderTexture(size_t n_samples = 8);

            /// @brief construct from internal
            MultisampledRenderTexture(detail::MultisampledRenderTextureInternal*);

            /// @brief destruct, frees all related OpenGL objects
            virtual ~MultisampledRenderTexture();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief bind for use as a texture, usually called automatically during mousetrap::Shape::render
            void bind() const override;

            /// @brief unbind for use as a texture, usually called automatically during mousetrap::Shape::render
            void unbind() const override;

            /// @brief make the textures framebuffer the current render buffer, anything rendered between this and mousetrap::MultisampledRenderTexture::unbind_as_render_target will appear in the textures buffer
            void bind_as_render_target() const;

            /// @brief unbind as render target, restores buffer that was active before mousetrap::MultisampledRenderTexture::bind_as_rendertarget was called
            void unbind_as_render_target() const;

            /// @brief create as texture of given size with all pixels set to RGBA(0, 0, 0, 0)
            /// @param width x-dimension
            /// @param height y-dimensino
            void create(size_t width, size_t height);

            /// @brief expose a gobject
            operator GObject*() const override;

        private:
            void free();
            detail::MultisampledRenderTextureInternal* _internal = nullptr;
    };
}