//
// Copyright 2022 Clemens Cords
// Created on 8/6/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/shader.hpp>
#if MOUSETRAP_ENABLE_OPENGL_COMPONENT

#include <iostream>
#include <mousetrap/texture.hpp>
#include <mousetrap/render_area.hpp>

namespace mousetrap
{
    namespace detail
    {
        DECLARE_NEW_TYPE(TextureInternal, texture_internal, TEXTURE_INTERNAL)

        static void texture_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_TEXTURE_INTERNAL(object);
            G_OBJECT_CLASS(texture_internal_parent_class)->finalize(object);

            if (detail::is_opengl_disabled())
                return;

            delete self->size;

            if (self->native_handle != 0)
                glDeleteTextures(1, &self->native_handle);
        }

        DEFINE_NEW_TYPE_TRIVIAL_INIT(TextureInternal, texture_internal, TEXTURE_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(TextureInternal, texture_internal, TEXTURE_INTERNAL)

        static TextureInternal* texture_internal_new()
        {
            auto* self = (TextureInternal*) g_object_new(texture_internal_get_type(), nullptr);
            texture_internal_init(self);

            if (detail::is_opengl_disabled())
            {
                log::critical("In render_area_internal_new: Trying to instantiate mousetrap::RenderArea, but the OpenGL component is disabled", MOUSETRAP_DOMAIN);
                return self;
            }

            self->native_handle = 0;
            self->wrap_mode = TextureWrapMode::REPEAT;
            self->scale_mode = TextureScaleMode::NEAREST;
            self->size = new Vector2i(0, 0);

            return self;
        }
    }
    
    Texture::Texture()
    {
        if (detail::is_opengl_disabled())
        {
            _internal = nullptr;
            return;
        }

        _internal = detail::texture_internal_new();
        g_object_ref(_internal);
        glGenTextures(1, &_internal->native_handle);
    }

    Texture::Texture(GLNativeHandle handle)
    {
        if (detail::is_opengl_disabled())
        {
            _internal = nullptr;
            return;
        }

        _internal = detail::texture_internal_new();
        g_object_ref(_internal);

        _internal->native_handle = handle;

        glBindTexture(GL_TEXTURE_2D, handle);

        int width = 0;
        int height = 0;

        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
        *_internal->size = {width, height};
    }

    Texture::Texture(detail::TextureInternal* internal)
    {
        if (detail::is_opengl_disabled())
        {
            _internal = nullptr;
            return;
        }

        _internal = g_object_ref(internal);
    }

    Texture::~Texture()
    {
        if (detail::is_opengl_disabled())
            return;

        g_object_unref(_internal);
    }

    NativeObject Texture::get_internal() const
    {
        if (detail::is_opengl_disabled())
            return nullptr;

        return G_OBJECT(_internal);
    }

    void Texture::create(uint64_t width, uint64_t height)
    {
        if (detail::is_opengl_disabled())
            return;

        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, _internal->native_handle);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glTexImage2D(GL_TEXTURE_2D,
             0,
             GL_RGBA16F,
             width,
             height,
             0,
             GL_RGBA,
             GL_UNSIGNED_BYTE,
             nullptr
        );

        *_internal->size = {width, height};
    }

    bool Texture::create_from_file(const std::string& path)
    {
        if (detail::is_opengl_disabled())
            return false;

        auto image = Image();
        auto out = image.create_from_file(path);

        create_from_image(image);
        return out;
    }

    Texture::Texture(Texture&& other) noexcept
    {
        if (detail::is_opengl_disabled())
        {
            _internal = nullptr;
            return;
        }

        _internal->native_handle = other._internal->native_handle;
        _internal->size = other._internal->size;
        _internal->wrap_mode = other._internal->wrap_mode;

        other._internal->native_handle = 0;
        *other._internal->size = {0, 0};
    }

    Texture& Texture::operator=(Texture&& other) noexcept
    {
        if (detail::is_opengl_disabled())
        {
            _internal = nullptr;
            return *this;
        }

        _internal->native_handle = other._internal->native_handle;
        _internal->size = other._internal->size;
        _internal->wrap_mode = other._internal->wrap_mode;

        other._internal->native_handle = 0;
        *other._internal->size = {0, 0};

        return *this;
    }

    void Texture::create_from_image(const Image& image)
    {
        if (detail::is_opengl_disabled())
            return;

        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, _internal->native_handle);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glTexImage2D(GL_TEXTURE_2D,
             0,
             GL_RGBA16F,
             image.get_size().x,
             image.get_size().y,
             0,
             GL_RGBA,
             GL_UNSIGNED_BYTE,
             image.data()
        );

        *_internal->size = image.get_size();
    }

    void Texture::bind(uint64_t texture_unit) const
    {
        if (detail::is_opengl_disabled())
            return;

        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D, _internal->native_handle);

        if (_internal->wrap_mode == TextureWrapMode::ZERO)
        {
            static float zero_border[] = {0.f, 0.f, 0.f, 0.f};
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, zero_border);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        }
        else if (_internal->wrap_mode == TextureWrapMode::ONE)
        {
            static float one_border[] = {1.f, 1.f, 1.f, 1.f};
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, one_border);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint) _internal->wrap_mode);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint) _internal->wrap_mode);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint) _internal->scale_mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint) _internal->scale_mode);
    }

    void Texture::bind() const
    {
        if (detail::is_opengl_disabled())
            return;

        bind(0);
    }

    void Texture::unbind() const
    {
        if (detail::is_opengl_disabled())
            return;

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::set_wrap_mode(TextureWrapMode wrap_mode)
    {
        if (detail::is_opengl_disabled())
            return;

        _internal->wrap_mode = wrap_mode;
    }

    TextureWrapMode Texture::get_wrap_mode()
    {
        if (detail::is_opengl_disabled())
            return TextureWrapMode::ZERO;

        return _internal->wrap_mode;
    }

    Vector2i Texture::get_size() const
    {
        if (detail::is_opengl_disabled())
            return {0, 0};

        return *_internal->size;
    }

    GLNativeHandle Texture::get_native_handle() const
    {
        if (detail::is_opengl_disabled())
            return 0;

        return _internal->native_handle;
    }

    void Texture::set_scale_mode(TextureScaleMode mode)
    {
        if (detail::is_opengl_disabled())
            return;

        _internal->scale_mode = mode;
    }

    TextureScaleMode Texture::get_scale_mode()
    {
        if (detail::is_opengl_disabled())
            return TextureScaleMode::NEAREST;

        return _internal->scale_mode;
    }

    Image Texture::download() const
    {
        if (detail::is_opengl_disabled())
            return Image();

        auto out = Image();
        out.create(_internal->size->x, _internal->size->y);

        glBindTexture(GL_TEXTURE_2D, _internal->native_handle);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, out.data());
        glBindTexture(GL_TEXTURE_2D, 0);

        return out;
    }

    Texture::operator GObject*() const
    {
        if (detail::is_opengl_disabled())
            return nullptr;

        return G_OBJECT(_internal);
    }
}

#endif // MOUSETRAP_ENABLE_OPENGL_COMPONENT