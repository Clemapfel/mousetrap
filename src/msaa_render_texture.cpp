//
// Created by clem on 2/23/23.
//

#include <mousetrap/gl_common.hpp>
#if MOUSETRAP_ENABLE_OPENGL_COMPONENT

#include <mousetrap/msaa_render_texture.hpp>
#include <mousetrap/log.hpp>

#include <iostream>

namespace mousetrap
{
    namespace detail
    {
        static void multisampled_render_texture_internal_free(MultisampledRenderTextureInternal* internal)
        {
            if (internal->buffer != 0)
                glDeleteFramebuffers(1, &internal->buffer);

            if (internal->msaa_color_buffer_texture != 0)
                glDeleteTextures(1, &internal->msaa_color_buffer_texture);

            if (internal->intermediate_buffer != 0)
                glDeleteFramebuffers(1, &internal->intermediate_buffer);

            if (internal->screen_texture != 0)
                glDeleteTextures(1, &internal->screen_texture);
        }
        
        DECLARE_NEW_TYPE(MultisampledRenderTextureInternal, multisampled_render_texture_internal, MULTISAMPLED_RENDER_TEXTURE_INTERNAL)

        static void multisampled_render_texture_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_MULTISAMPLED_RENDER_TEXTURE_INTERNAL(object);
            G_OBJECT_CLASS(multisampled_render_texture_internal_parent_class)->finalize(object);
            multisampled_render_texture_internal_free(self);
        }

        DEFINE_NEW_TYPE_TRIVIAL_INIT(MultisampledRenderTextureInternal, multisampled_render_texture_internal, MULTISAMPLED_RENDER_TEXTURE_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(MultisampledRenderTextureInternal, multisampled_render_texture_internal, MULTISAMPLED_RENDER_TEXTURE_INTERNAL)

        static MultisampledRenderTextureInternal* multisampled_render_texture_internal_new()
        {
            auto* self = (MultisampledRenderTextureInternal*) g_object_new(multisampled_render_texture_internal_get_type(), nullptr);
            multisampled_render_texture_internal_init(self);
            
            return self;
        }
    }
    
    MultisampledRenderTexture::MultisampledRenderTexture(size_t n_samples)
    {
        _internal = detail::multisampled_render_texture_internal_new();
        _internal->n_samples = n_samples;
    }

    MultisampledRenderTexture::MultisampledRenderTexture(detail::MultisampledRenderTextureInternal* internal)
    {
        _internal = g_object_ref(_internal);
    }

    MultisampledRenderTexture::~MultisampledRenderTexture()
    {
        g_object_unref(_internal);
    }

    NativeObject MultisampledRenderTexture::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    void MultisampledRenderTexture::create(size_t width, size_t height)
    {
        free();

        _internal->width = width;
        _internal->height = height;

        GLint before = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &before);

        glGenFramebuffers(1, &_internal->buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, _internal->buffer);

        glGenTextures(1, &_internal->msaa_color_buffer_texture);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _internal->msaa_color_buffer_texture);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, _internal->n_samples, GL_RGBA, width, height, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, _internal->msaa_color_buffer_texture, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glGenFramebuffers(1, &_internal->intermediate_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, _internal->intermediate_buffer);

        glGenTextures(1, &_internal->screen_texture);
        glBindTexture(GL_TEXTURE_2D, _internal->screen_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _internal->screen_texture, 0);	// we only need a color buffer

        glBindFramebuffer(GL_FRAMEBUFFER, before);
    }

    void MultisampledRenderTexture::bind_as_render_target() const
    {
        if (_internal->width == 0 or _internal->height == 0)
        {
            log::critical("In MultisampledRenderTexture::bind_as_rendertarget: Framebuffes uninitialized, call `MultisampledRenderTexture::create` first", MOUSETRAP_DOMAIN);
        }

        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_internal->before_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, _internal->buffer);
    }

    void MultisampledRenderTexture::unbind_as_render_target() const
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, _internal->buffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _internal->intermediate_buffer);
        glBlitFramebuffer(0, 0, _internal->width, _internal->height, 0, 0, _internal->width, _internal->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, _internal->before_buffer);
    }

    void MultisampledRenderTexture::bind() const
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _internal->screen_texture);
    }

    void MultisampledRenderTexture::unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    MultisampledRenderTexture::operator GObject*() const
    {
        return G_OBJECT(_internal);
    }

    void MultisampledRenderTexture::free()
    {
        if (_internal->buffer != 0)
            glDeleteFramebuffers(1, &_internal->buffer);

        if (_internal->msaa_color_buffer_texture != 0)
            glDeleteTextures(1, &_internal->msaa_color_buffer_texture);

        if (_internal->intermediate_buffer != 0)
            glDeleteFramebuffers(1, &_internal->intermediate_buffer);

        if (_internal->screen_texture != 0)
            glDeleteTextures(1, &_internal->screen_texture);
    }
}

#endif // MOUSETRAP_ENABLE_OPENGL_COMPONENT