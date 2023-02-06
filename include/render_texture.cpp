#include <app/render_texture.hpp>

namespace mousetrap
{
    RenderTexture::RenderTexture()
            : Texture()
    {
        glGenFramebuffers(1, &_framebuffer_handle);
        glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer_handle);
    }

    RenderTexture::~RenderTexture() noexcept
    {
        if (_framebuffer_handle != 0)
            glDeleteFramebuffers(1, &_framebuffer_handle);
    }

    RenderTexture::RenderTexture(RenderTexture&& other)
    {
        this->_framebuffer_handle = other._framebuffer_handle;
        other._framebuffer_handle = 0;
    }

    RenderTexture& RenderTexture::operator=(RenderTexture&& other)
    {
        this->_framebuffer_handle = other._framebuffer_handle;
        other._framebuffer_handle = 0;
        return *this;
    }

    void RenderTexture::bind_as_rendertarget() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer_handle);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, get_native_handle(), 0);
        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, DrawBuffers);
    }

    void RenderTexture::unbind_as_rendertarget() const
    {
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}