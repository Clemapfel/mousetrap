//
// Created by clem on 2/23/23.
//

#include <app/msaa_texture.hpp>

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
        
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        glGenTextures(1, &textureColorBufferMultiSampled);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, _n_samples, GL_RGB, width, height, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);

        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, _n_samples, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        glGenFramebuffers(1, &intermediateFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

        glGenTextures(1, &screenTexture);
        glBindTexture(GL_TEXTURE_2D, screenTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);	// we only need a color buffer

        glBindFramebuffer(GL_FRAMEBUFFER, before);
    }

    void MultisampledRenderTexture::free()
    {
        if (framebuffer != 0)
            glDeleteFramebuffers(1, &framebuffer);

        if (textureColorBufferMultiSampled != 0)
            glDeleteTextures(1, &textureColorBufferMultiSampled);

        if (rbo != 0)
            glDeleteRenderbuffers(1, &rbo);

        if (intermediateFBO != 0)
            glDeleteFramebuffers(1, &intermediateFBO);

        if (screenTexture != 0)
            glDeleteTextures(1, &screenTexture);
    }

    MultisampledRenderTexture::~MultisampledRenderTexture()
    {
        free();
    }

    void MultisampledRenderTexture::bind_as_rendertarget() const 
    {
        if (_width == 0 or _height == 0)
            std::cerr << "[WARNING] In MultisampledRenderTexture::bind_as_rendertarget: Framebuffes uninitialized, call `MultisampledRenderTexture::create` first" << std::endl;

        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_before_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    }

    void MultisampledRenderTexture::unbind_as_rendertarget() const 
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
        glBlitFramebuffer(0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, _before_buffer);
    }

    void MultisampledRenderTexture::bind() const 
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screenTexture);
    }

    void MultisampledRenderTexture::unbind() const 
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}