#include <mousetrap/gl_common.hpp>

#if MOUSETRAP_ENABLE_OPENGL_COMPONENT

#include <mousetrap/render_texture.hpp>
#include <mousetrap/render_area.hpp>

namespace mousetrap
{
    namespace detail
    {
        DECLARE_NEW_TYPE(RenderTextureInternal, render_texture_internal, RENDER_TEXTURE_INTERNAL)

        static void render_texture_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_RENDER_TEXTURE_INTERNAL(object);
            G_OBJECT_CLASS(render_texture_internal_parent_class)->finalize(object);

            if (detail::is_opengl_disabled())
                return;

            if (self->framebuffer_handle != 0)
                glDeleteFramebuffers(1, &self->framebuffer_handle);
        }

        DEFINE_NEW_TYPE_TRIVIAL_INIT(RenderTextureInternal, render_texture_internal, RENDER_TEXTURE_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(RenderTextureInternal, render_texture_internal, RENDER_TEXTURE_INTERNAL)

        static RenderTextureInternal* render_texture_internal_new()
        {
            auto* self = (RenderTextureInternal*) g_object_new(render_texture_internal_get_type(), nullptr);
            render_texture_internal_init(self);

            if (detail::is_opengl_disabled())
                return self;

            glGenFramebuffers(1, &self->framebuffer_handle);
            glBindFramebuffer(GL_FRAMEBUFFER, self->framebuffer_handle);

            return self;
        }
    }
    
    RenderTexture::RenderTexture()
        : Texture()
    {
        if (detail::is_opengl_disabled())
        {
            _internal = nullptr;
            return;
        }

        _internal = detail::render_texture_internal_new();
        detail::attach_ref_to(Texture::operator GObject*(), _internal);
        g_object_ref(_internal);
    }

    RenderTexture::RenderTexture(detail::RenderTextureInternal* internal)
    {
        if (detail::is_opengl_disabled())
        {
            _internal = nullptr;
            return;
        }

        _internal = g_object_ref(internal);
    }

    RenderTexture::~RenderTexture()
    {
        if (detail::is_opengl_disabled())
            return;

        g_object_unref(_internal);
    }

    NativeObject RenderTexture::get_internal() const
    {
        if (detail::is_opengl_disabled())
            return nullptr;

        return G_OBJECT(_internal);
    }

    GLNativeHandle RenderTexture::get_native_handle() const
    {
        if (detail::is_opengl_disabled())
            return 0;

        return _internal->framebuffer_handle;
    }

    RenderTexture::RenderTexture(RenderTexture&& other)
    {
        if (detail::is_opengl_disabled())
        {
            _internal = nullptr;
            return;
        }

        _internal->framebuffer_handle = other._internal->framebuffer_handle;
        other._internal->framebuffer_handle = 0;
    }

    RenderTexture& RenderTexture::operator=(RenderTexture&& other)
    {
        if (detail::is_opengl_disabled())
        {
            _internal = nullptr;
            return *this;
        }

        _internal->framebuffer_handle = other._internal->framebuffer_handle;
        other._internal->framebuffer_handle = 0;
        return *this;
    }

    void RenderTexture::bind_as_render_target() const
    {
        if (detail::is_opengl_disabled())
            return;

        constexpr auto ATTACHMENT = GL_COLOR_ATTACHMENT5;

        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_internal->before_buffer);

        glBindFramebuffer(GL_FRAMEBUFFER, _internal->framebuffer_handle);
        glFramebufferTexture2D(GL_FRAMEBUFFER, ATTACHMENT, GL_TEXTURE_2D, get_native_handle(), 0);
        GLenum DrawBuffers[1] = {ATTACHMENT};
        glDrawBuffers(1, DrawBuffers);
    }

    void RenderTexture::unbind_as_render_target() const
    {
        if (detail::is_opengl_disabled())
            return;

        glBindFramebuffer(GL_FRAMEBUFFER, _internal->before_buffer);
    }

    RenderTexture::operator GObject*() const
    {
        if (detail::is_opengl_disabled())
            return nullptr;

        return G_OBJECT(_internal);
    }
}

#endif // MOUSETRAP_ENABLE_OPENGL_COMPONENT