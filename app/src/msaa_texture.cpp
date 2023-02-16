//
// Created by clem on 2/15/23.
//

#include <app/msaa_texture.hpp>

namespace mousetrap
{
    MSAATexture::MSAATexture()
    {
        _area.connect_signal_realize(on_area_realize, this);
        _area.connect_signal_resize(on_area_resize, this);
        _area.connect_signal_render(on_area_render, this);
        _area.set_size_request({800, 800});
        _area.add_tick_callback([](FrameClock clock, MSAATexture* instance) {

            if (not instance->_area.get_is_realized())
                return true;

            instance->_transform->rotate(degrees(0.1), {0, 0});
            instance->_area.queue_render();
            return true;
        }, this);
    }

    MSAATexture::operator Widget*()
    {
        return &_area;
    }

    void MSAATexture::on_area_realize(Widget* widget, MSAATexture* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        auto as_ellipse = [](Vector2f center, float x_radius, float y_radius, size_t n_vertices){

            const float step = 360.f / n_vertices;

            std::vector<Vector2f> out;

            for (float angle = 0; angle < 360; angle += step)
            {
                auto as_radians = angle * M_PI / 180.f;
                out.emplace_back(
                        center.x + cos(as_radians) * x_radius,
                        center.y + sin(as_radians) * y_radius
                );
            }

            return out;
        };

        instance->_shader = new Shader();
        instance->_shape = new Shape();
        instance->_shape->as_wireframe(as_ellipse({0.5, 0.5}, 0.4, 0.4, 11));
        instance->_shape->set_color(RGBA(1, 0, 1, 1));

        instance->_render_shader = new Shader();
        instance->_render_shader->create_from_file(get_resource_path() + "shaders/msaa_render.frag", ShaderType::FRAGMENT);
        instance->_render_shape = new Shape();
        instance->_render_shape->as_rectangle({0, 0}, {1, 1});

        instance->realize();
        area->queue_render();
    }

    void MSAATexture::on_area_resize(GLArea* area, int w, int h, MSAATexture* instance)
    {
        instance->_canvas_size ={w, h};
        area->queue_render();
    }

    bool MSAATexture::on_area_render(GLArea* area, GdkGLContext*, MSAATexture* instance)
    {
        area->make_current();
        instance->render();
        return true;
    }

    void MSAATexture::realize()
    {
        size_t n_samples = 8;

        glEnable(GL_MULTISAMPLE);

        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        // create a multisampled color attachment texture

        glGenTextures(1, &textureColorBufferMultiSampled);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, n_samples, GL_RGB, _canvas_size.x, _canvas_size.y, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);
        // create a (also multisampled) renderbuffer object for depth and stencil attachments

        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, n_samples, GL_DEPTH24_STENCIL8, _canvas_size.x, _canvas_size.y);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // configure second post-processing framebuffer
        glGenFramebuffers(1, &intermediateFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
        // create a color attachment texture

        glGenTextures(1, &screenTexture);
        glBindTexture(GL_TEXTURE_2D, screenTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _canvas_size.x, _canvas_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);	// we only need a color buffer

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void MSAATexture::render()
    {
        static GLNativeHandle before = [](){
            GLint before = 0;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &before);
            return before;
        }();

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. draw scene as normal in multisampled buffers
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        _shape->render(*_shader, *_transform);

        // 2. now blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
        glBlitFramebuffer(0, 0, _canvas_size.x, _canvas_size.y, 0, 0, _canvas_size.x, _canvas_size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // 3. now render quad with scene's visuals as its texture image

        glBindFramebuffer(GL_FRAMEBUFFER, before);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        // draw Screen quad
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screenTexture);

        _render_shape->render(*_render_shader, *_render_transform);
        glFlush();

    }
}
