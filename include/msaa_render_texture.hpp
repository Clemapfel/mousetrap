//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 2/15/23
//

#pragma once

#include <include/gl_common.hpp>
#include <include/texture_object.hpp>

namespace mousetrap
{
    /// @brief render texture that utilizes a multi-sampled buffer, which renders the resulting image anti-aliased
    class MultisampledRenderTexture : public TextureObject
    {
        public:
            /// @brief construct
            /// @param n_samples number of MSAA samples, usually 2, 4, 8 or 16
            MultisampledRenderTexture(size_t n_samples = 8);

            /// @brief destruct, frees all reated OpenGL objects
            virtual ~MultisampledRenderTexture();

            /// @brief bind for use as a texture, usually called automatically during mousetrap::Shape::render
            void bind() const override;

            /// @brief unbind for use as a texture, usually called automatically during mousetrap::Shape::render
            void unbind() const override;

            /// @brief make the textures framebuffer the current render buffer, anything rendered between this and mousetrap::MultisampledRenderTexture::unbind_as_render_target will appear in the textures buffer
            void bind_as_rendertarget() const;

            /// @brief unbind as render target, restores buffer that was active before mousetrap::MultisampledRenderTexture::bind_as_rendertarget was called
            void unbind_as_rendertarget() const;

            /// @brief create as texture of given size with all pixels set to RGBA(0, 0, 0, 0)
            /// @param width x-dimension
            /// @param height y-dimensino
            void create(size_t width, size_t height);

        private:
            size_t _n_samples;

            size_t _width = 0;
            size_t _height = 0;

            mutable GLint _before_buffer = 0;

            GLNativeHandle _buffer = 0;
            GLNativeHandle _msaa_color_buffer_texture = 0;
            GLNativeHandle _intermediate_buffer = 0;
            GLNativeHandle _screen_texture = 0;

            void free();
    };
}