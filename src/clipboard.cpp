//
// Created by clem on 3/3/23.
//

#include <include/clipboard.hpp>
#include <include/file_system.hpp>

namespace mousetrap::detail
{
    DECLARE_NEW_TYPE(ClipboardInternal, clipboard_internal, CLIPBOARD_INTERNAL)
    DEFINE_NEW_TYPE_TRIVIAL_FINALIZE(ClipboardInternal, clipboard_internal, CLIPBOARD_INTERNAL)
    DEFINE_NEW_TYPE_TRIVIAL_INIT(ClipboardInternal, clipboard_internal, CLIPBOARD_INTERNAL)
    DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(ClipboardInternal, clipboard_internal, CLIPBOARD_INTERNAL)

    static ClipboardInternal* clipboard_internal_new(GtkWidget* widget)
    {
        auto* self = (ClipboardInternal*) g_object_new(clipboard_internal_get_type(), nullptr);

        self->native = gtk_widget_get_clipboard(widget);
        self->get_string_f = nullptr;
        self->get_image_f = nullptr;

        attach_ref_to(G_OBJECT(widget), self);

        return self;
    }
}

namespace mousetrap
{
    Clipboard::Clipboard(const Widget* widget)
        : _internal(detail::clipboard_internal_new(widget != nullptr ? widget->operator GtkWidget*() : nullptr))
    {}
    
    Clipboard::Clipboard(detail::ClipboardInternal* internal) 
        : _internal(internal)
    {}

    Clipboard::operator GdkClipboard*()
    {
        return _internal->native;
    }

    bool Clipboard::is_local() const
    {
        return gdk_clipboard_is_local(_internal->native);
    }

    bool Clipboard::contains_string() const
    {
        auto* formats = gdk_clipboard_get_formats(_internal->native);
        return gdk_content_formats_contain_gtype(formats, G_TYPE_STRING);
    }

    void Clipboard::set_string(const std::string& str)
    {
        gdk_clipboard_set_text(_internal->native, str.c_str());
    }

    void Clipboard::get_string_callback_wrapper(GObject* clipboard, GAsyncResult* result, detail::ClipboardInternal* data)
    {
        GError* error = nullptr;
        auto* text = gdk_clipboard_read_text_finish(GDK_CLIPBOARD(clipboard), result, &error);

        auto* instance = detail::MOUSETRAP_CLIPBOARD_INTERNAL(data);
        std::string str;

        if (error == nullptr)
            str = std::string(text);
        else
            g_error_free(error);

        if (instance->get_string_f != nullptr)
        {
            auto temp = Clipboard(instance);
            instance->get_string_f(temp, str);
        }
        else
            log::critical("In Clipboard::get_string_callback_wrapper: String succesfully read but no valid handler function is available", MOUSETRAP_DOMAIN);
    }

    bool Clipboard::contains_image() const
    {
        auto* formats = gdk_clipboard_get_formats(_internal->native);
        return gdk_content_formats_contain_gtype(formats, GDK_TYPE_TEXTURE);
    }

    bool Clipboard::contains_file() const
    {
        auto* formats = gdk_clipboard_get_formats(_internal->native);
        return gdk_content_formats_contain_gtype(formats, G_TYPE_FILE);
    }

    void Clipboard::set_image(const Image& image)
    {
        auto* pixbuf = image.operator GdkPixbuf*();
        auto* texture = gdk_texture_new_for_pixbuf(pixbuf);
        gdk_clipboard_set_texture(_internal->native, texture);
    }

    void Clipboard::set_file(const FileDescriptor& file)
    {
        gdk_clipboard_set(_internal->native, G_TYPE_FILE, file.operator GFile*());
    }

    void Clipboard::get_image_callback_wrapper(GObject* clipboard, GAsyncResult* result, detail::ClipboardInternal* self)
    {
        GError* error = nullptr;
        auto* texture = gdk_clipboard_read_texture_finish(GDK_CLIPBOARD(clipboard), result, &error);

        auto* instance = (detail::ClipboardInternal*) self;
        auto image = Image();

        if (error != nullptr)
        {
            auto size = Vector2ui(gdk_texture_get_width(texture), gdk_texture_get_height(texture));

            // FORMAT: ARGB32, c.f. https://docs.gtk.org/gdk4/method.Texture.download.html

            auto* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, gdk_texture_get_width(texture), gdk_texture_get_height(texture));
            gdk_texture_download(texture, cairo_image_surface_get_data(surface), cairo_image_surface_get_stride(surface));

            auto* data = cairo_image_surface_get_data(surface);
            auto format = cairo_image_surface_get_format(surface);

            image.create(size.x, size.y);
            for (size_t i = 0; i < size.x * size.y * 4; i = i + 4)
            {
                guchar b = data[i + 0];
                guchar g = data[i + 1];
                guchar r = data[i + 2];
                guchar a = data[i + 3];

                image.set_pixel(i / 4, RGBA(r / 255.f, g / 255.f, b / 255.f, a / 255.f));
            }

            cairo_surface_mark_dirty(surface);
            g_free(surface);
        }
        else
            g_error_free(error);

        if (instance->get_image_f != nullptr)
        {
            auto temp = Clipboard(instance);
            instance->get_image_f(temp, image);
        }
        else
            log::critical("In Clipboard::get_image_callback_wrapper: Image succesfully read but no valid handler function is available", MOUSETRAP_DOMAIN);
    }
}