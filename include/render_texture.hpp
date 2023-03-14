//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 2/6/23
//

#pragma once

#include "texture.hpp"

namespace mousetrap
{
    class RenderTexture : public Texture
    {
        public:
            RenderTexture();
            ~RenderTexture();

            RenderTexture(const RenderTexture&) = delete;
            RenderTexture& operator=(const RenderTexture&) = delete;

            RenderTexture(RenderTexture&&);
            RenderTexture& operator=(RenderTexture&&);

            void bind_as_rendertarget() const;
            void unbind_as_rendertarget() const;

        protected:
            GLNativeHandle _framebuffer_handle;
            mutable GLint _before_buffer = 0;
    };
}

