//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <include/entry.hpp>

namespace mousetrap
{
    Entry::Entry()
        : WidgetImplementation<GtkEntry>(GTK_ENTRY(gtk_entry_new())),
          CTOR_SIGNAL(Entry, activate),
          CTOR_SIGNAL(Entry, text_changed)
    {}

    std::string Entry::get_text() const
    {
        auto* buffer = gtk_entry_get_buffer(get_native());
        return gtk_entry_buffer_get_text(buffer);
    }

    void Entry::set_text(const std::string& text)
    {
        auto* buffer = gtk_entry_get_buffer(get_native());
        gtk_entry_buffer_set_text(buffer, text.c_str(), text.size());
        gtk_entry_buffer_emit_inserted_text(buffer, 0, text.c_str(), text.size());
        gtk_entry_set_buffer(get_native(), buffer);
    }

    void Entry::set_has_frame(bool b)
    {
        gtk_entry_set_has_frame(get_native(), b);
    }

    bool Entry::get_has_frame() const
    {
        return gtk_entry_get_has_frame(get_native());
    }

    void Entry::set_max_length(int n)
    {
        gtk_entry_set_max_length(get_native(), n);
    }

    int Entry::get_max_length() const
    {
        return gtk_entry_get_max_length(get_native());
    }

    void Entry::set_text_visible(bool b)
    {
        gtk_entry_set_visibility(get_native(), b);
    }

    bool Entry::get_text_visible() const
    {
        return gtk_entry_get_visibility(get_native());
    }
}
