//
// Created by clem on 3/3/23.
//

#include <app/clipboard.hpp>
#include <include/get_resource_path.hpp>

namespace mousetrap
{
    Clipboard::Clipboard(GdkClipboard* clipboard)
        : _native(clipboard)
    {}

    Clipboard::Clipboard(Widget* widget)
        : Clipboard(gtk_widget_get_clipboard(widget->operator GtkWidget *()))
    {}

    Clipboard::operator GdkClipboard*()
    {
        return _native;
    }

    bool Clipboard::contains_string() const
    {
        auto* formats = gdk_clipboard_get_formats(_native);
        return gdk_content_formats_contain_gtype(formats, G_TYPE_STRING);
    }

    void Clipboard::set_string(const std::string& str)
    {
        gdk_clipboard_set_text(_native, str.c_str());
    }

    void Clipboard::get_string_callback_wrapper(GObject* clipboard, GAsyncResult* result, gpointer data)
    {
        GError* error = nullptr;
        auto* text = gdk_clipboard_read_text_finish(GDK_CLIPBOARD(clipboard), result, &error);

        if (error != nullptr)
        {
            std::cerr << "[ERROR] In Clipboard::get_string_callback: " << error->message << std::endl;
            return;
        }

        Clipboard* instance = (Clipboard*) data;
        auto str = std::string(text != nullptr ? text : "");

        if (instance->get_string_f != nullptr)
            instance->get_string_f(instance, text);
    }

    bool Clipboard::contains_image() const
    {
        auto* formats = gdk_clipboard_get_formats(_native);
        return gdk_content_formats_contain_gtype(formats, GDK_TYPE_TEXTURE);
    }

    void Clipboard::set_image(const Image& image)
    {
        auto* pixbuf = image.to_pixbuf();
        auto* texture = gdk_texture_new_for_pixbuf(pixbuf);
        gdk_clipboard_set_texture(_native, texture);
    }

    void Clipboard::get_image_callback_wrapper(GObject* clipboard, GAsyncResult* result, gpointer data)
    {
        GError* error = nullptr;
        auto* texture = gdk_clipboard_read_texture_finish(GDK_CLIPBOARD(clipboard), result, &error);

        if (error != nullptr)
        {
            std::cerr << "[ERROR] In Clipboard::get_image_callback: " << error->message << std::endl;
            return;
        }

        Clipboard* instance = (Clipboard*) data;
        auto image = Image();
        image.create_from_texture(texture);
        std::cout << image.save_to_file(get_resource_path() + "temp.png") << std::endl;
        exit(0);

        //auto* pixbuf = gdk_pixbuf_new_from_stream(stream, nullptr, error);
        //auto image = Image(pixbuf);

        //if (instance->get_image_f != nullptr)
          //  instance->get_image_f(instance, image);
    }
}