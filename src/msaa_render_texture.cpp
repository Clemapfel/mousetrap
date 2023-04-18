//
// Created by clem on 2/23/23.
//

#include <include/msaa_render_texture.hpp>
#include <include/log.hpp>

#include <iostream>

namespace mousetrap
{
    MultisampledRenderTexture::MultisampledRenderTexture(size_t n_samples)
        : _n_samples(n_samples)
    {}

    void MultisampledRenderTexture::create(size_t width, size_t height)
    {
        free();

        _width = width;
        _height = height;

        GLint before = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &before);

        glGenFramebuffers(1, &_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, _buffer);

        glGenTextures(1, &_msaa_color_buffer_texture);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _msaa_color_buffer_texture);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, _n_samples, GL_RGBA, width, height, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, _msaa_color_buffer_texture, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glGenFramebuffers(1, &_intermediate_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, _intermediate_buffer);

        glGenTextures(1, &_screen_texture);
        glBindTexture(GL_TEXTURE_2D, _screen_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _screen_texture, 0);	// we only need a color buffer

        glBindFramebuffer(GL_FRAMEBUFFER, before);
    }

    void MultisampledRenderTexture::free()
    {
        if (_buffer != 0)
            glDeleteFramebuffers(1, &_buffer);

        if (_msaa_color_buffer_texture != 0)
            glDeleteTextures(1, &_msaa_color_buffer_texture);

        if (_intermediate_buffer != 0)
            glDeleteFramebuffers(1, &_intermediate_buffer);

        if (_screen_texture != 0)
            glDeleteTextures(1, &_screen_texture);
    }

    MultisampledRenderTexture::~MultisampledRenderTexture()
    {
        free();
    }

    void MultisampledRenderTexture::bind_as_rendertarget() const
    {
        if (_width == 0 or _height == 0)
        {
            log::critical("In MultisampledRenderTexture::bind_as_rendertarget: Framebuffes uninitialized, call `MultisampledRenderTexture::create` first", MOUSETRAP_DOMAIN);
        }

        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_before_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, _buffer);
    }

    void MultisampledRenderTexture::unbind_as_rendertarget() const
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, _buffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _intermediate_buffer);
        glBlitFramebuffer(0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, _before_buffer);
    }

    void MultisampledRenderTexture::bind() const
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _screen_texture);
    }

    void MultisampledRenderTexture::unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}