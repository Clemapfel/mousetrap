//
// Created by clem on 3/3/23.
//

#include <app/clipboard.hpp>

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
        auto str = std::string(text);

        if (instance->get_string_f != nullptr)
            instance->get_string_f(instance, text);
    }
}