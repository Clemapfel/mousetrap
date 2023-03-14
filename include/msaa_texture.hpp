//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 2/15/23
//

#pragma once

#include "mousetrap.hpp"

namespace mousetrap
{
    class MultisampledRenderTexture : public TextureObject
    {
        public:
            MultisampledRenderTexture(size_t n_samples = 8);
            virtual ~MultisampledRenderTexture();

            void bind() const override;
            void unbind() const override;

            void bind_as_rendertarget() const;
            void unbind_as_rendertarget() const;

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