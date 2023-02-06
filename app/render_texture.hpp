//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 2/6/23
//

#pragma once

#include <include/texture.hpp>

namespace mousetrap
{
    // to render to a texture, the regular elements need to have the
    // fragment shader output routed to location 0, see resoures/to_render_texture.frag

    class RenderTexture : public Texture
    {
        public:
            RenderTexture();
            virtual ~RenderTexture();

            RenderTexture(const RenderTexture&) = delete;
            RenderTexture& operator=(const RenderTexture&) = delete;

            RenderTexture(RenderTexture&&);
            RenderTexture& operator=(RenderTexture&&);

            void bind_as_rendertarget() const;
            void unbind_as_rendertarget() const;

        protected:
            GLNativeHandle _framebuffer_handle;
    };
}

