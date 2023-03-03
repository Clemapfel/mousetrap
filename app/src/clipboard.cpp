//
// Created by clem on 3/3/23.
//

#include <app/clipboard.hpp>

namespace mousetrap
{
    ContentFormats::ContentFormats(GdkContentFormats* native)
        : _native(native)
    {}

    void ContentFormats::from_string(const std::string& in)
    {
        _native = gdk_content_formats_parse(in.c_str());
    }

    std::string ContentFormats::to_string() const
    {
        return std::string(gdk_content_formats_to_string(_native));
    }

    bool ContentFormats::operator==(const ContentFormats& other)
    {
        return gdk_content_formats_match(_native, other._native);
    }

    bool ContentFormats::operator!=(const ContentFormats& other)
    {
        return not (*this == other);
    }

    Clipboard::Clipboard(GdkClipboard* native)
        : _native(native)
    {}

    ContentFormats Clipboard::get_formats() const
    {
        return ContentFormats(gdk_clipboard_get_formats(_native));
    }

    bool Clipboard::is_local() const
    {
        return gdk_clipboard_is_local(_native);
    }

    void Clipboard::set_image(const Image& image)
    {
        auto* pixbuf = image.to_pixbuf();
        auto* texture = gdk_texture_new_for_pixbuf(pixbuf);
        gdk_clipboard_set_texture(_native, texture);
        g_object_unref(pixbuf);
        g_object_unref(texture);
    }

    void Clipboard::set_text(const std::string& text)
    {
        gdk_clipboard_set_text(_native, text.c_str());
    }

    void Clipboard::read_text_callback(GObject* clipboard, GAsyncResult* result, gpointer instance)
    {
        GError* error = nullptr;
        auto* text = gdk_clipboard_read_text_finish(GDK_CLIPBOARD(clipboard), result, &error);

        if (error == nullptr and text != nullptr)
        {
            std::cout << "success" << std::endl;
            //std::cout << text << std::endl;
        }

        //g_free(text);
    }

    static void
    text_loaded_cb (GObject      *clipboard,
    GAsyncResult *res,
    gpointer      data)
    {
        GError *error = NULL;
        char *text;

        text = gdk_clipboard_read_text_finish (GDK_CLIPBOARD (clipboard), res, &error);
        if (text == NULL)
        {
            g_print ("%s\n", error->message);
            g_error_free (error);
            return;
        }

        std::cout << text << std::endl;
        g_free (text);
    }


    std::string Clipboard::read_text() const
    {
        gdk_clipboard_read_text_async(_native, nullptr, text_loaded_cb, (gpointer) nullptr);
    }
}