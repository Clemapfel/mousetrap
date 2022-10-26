// 
// Copyright 2022 Clemens Cords
// Created on 10/7/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

#include <include/texture.hpp>
#include <include/blend_mode.hpp>
#include <include/shape.hpp>

namespace mousetrap
{
    // hardware-accelerated image processing
    class TextureWorkspace
    {
        public:
            TextureWorkspace(GdkGLContext* context);
            ~TextureWorkspace();

            Image* merge(Texture* upper, Texture* lower, BlendMode);

        private:
            GdkGLContext* _context;
            GLNativeHandle _framebuffer;

            Shape* _shape_upper;
            Shape* _shape_lower;

            static inline Shader* noop_shader = nullptr;
            static inline GLTransform* noop_transform = nullptr;
    };
}

///

namespace mousetrap
{
    TextureWorkspace::TextureWorkspace(GdkGLContext* context)
        : _context(context)
    {
        gdk_gl_context_make_current(context);
        glGenFramebuffers(1, &_framebuffer);
        _shape_upper = new Shape();
        _shape_upper->as_rectangle({0, 0}, {1, 1});

        _shape_lower = new Shape();
        _shape_lower->as_rectangle({0, 0}, {1, 1});

        if (noop_transform == nullptr)
            noop_transform = new GLTransform();

        if (noop_shader == nullptr)
            noop_shader = new Shader();
    }

    TextureWorkspace::~TextureWorkspace()
    {
        delete _shape_upper;
        delete _shape_lower;
        glDeleteBuffers(1, &_framebuffer);
    }

    Image* TextureWorkspace::merge(Texture* upper, Texture* lower, BlendMode mode)
    {
        _shape_upper->set_texture(upper);
        _shape_lower->set_texture(lower);

        gdk_gl_context_make_current(_context);
        assert(upper->get_size() == lower->get_size());
        glViewport(0, 0, upper->get_size().x, upper->get_size().y);
        glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_BLEND);
        set_current_blend_mode(mode);

        _shape_upper->set_texture(upper);
        _shape_lower->set_texture(lower);

        _shape_upper->render(*noop_shader, *noop_transform);
        _shape_lower->render(*noop_shader, *noop_transform);

        glFlush();

        auto* out = new Image();
        out->create(upper->get_size().x, lower->get_size().y, RGBA(0, 0, 0, 0));

        glReadPixels(0, 0, out->get_size().x, out->get_size().y, GL_RGBA, GL_FLOAT, out->data());
        return out;
    }
}