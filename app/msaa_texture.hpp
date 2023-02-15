//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 2/15/23
//

#pragma once

#include <mousetrap.hpp>

namespace mousetrap
{
    class MSAATexture
    {
        public:
            MSAATexture();
            operator Widget*();

        private:
            GLArea _area;

            Vector2f _canvas_size = {1, 1};
            static void on_area_realize(Widget* widget, MSAATexture* instance);
            static void on_area_resize(GLArea*, int w, int h, MSAATexture* instance);
            static bool on_area_render(GLArea*, GdkGLContext*, MSAATexture* instance);

            float _current_angle_dg = 0;
            GLTransform* _transform = new GLTransform();
            Shape* _shape;
            Shader* _shader;

            GLTransform* _render_transform = new GLTransform();
            Shader* _render_shader;
            Shape* _render_shape;

            ///

            GLNativeHandle framebuffer;
            GLNativeHandle textureColorBufferMultiSampled;
            GLNativeHandle intermediateFBO;
            GLNativeHandle screenTexture;
            void realize();
            void render();

            static inline const int SCR_WIDTH = 800;
            static inline const int SCR_HEIGHT = 800;
    };
}