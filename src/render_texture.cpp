#include <include/render_texture.hpp>

namespace mousetrap
{
    RenderTexture::RenderTexture()
        : Texture()
    {
        glGenFramebuffers(1, &_framebuffer_handle);
        glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer_handle);
    }

    RenderTexture::~RenderTexture()
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
        constexpr auto ATTACHMENT = GL_COLOR_ATTACHMENT5;

        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_before_buffer);

        glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer_handle);
        glFramebufferTexture2D(GL_FRAMEBUFFER, ATTACHMENT, GL_TEXTURE_2D, get_native_handle(), 0);
        GLenum DrawBuffers[1] = {ATTACHMENT};
        glDrawBuffers(1, DrawBuffers);
    }

    void RenderTexture::unbind_as_rendertarget() const
    {
        //glBindFramebuffer(GL_FRAMEBUFFER, _before_buffer);
    }
}