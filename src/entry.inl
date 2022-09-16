// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Entry::Entry()
    {
        _buffer = gtk_entry_buffer_new(nullptr, 0);
        _entry = GTK_ENTRY(gtk_entry_new_with_buffer(_buffer));
    }

    Entry::operator GtkWidget*()
    {
        return GTK_WIDGET(_entry);
    }

    void Entry::set_has_frame(bool b)
    {
        gtk_entry_set_has_frame(_entry, b);
    }

    std::string Entry::get_text() const
    {
        auto* buffer = gtk_entry_get_buffer(_entry);
        return gtk_entry_buffer_get_text(buffer);
    }

    void Entry::set_text(const std::string& text)
    {
        auto* buffer = gtk_entry_get_buffer(_entry);
        gtk_entry_buffer_set_text(buffer, text.c_str(), text.size());
        gtk_entry_buffer_emit_inserted_text(buffer, 0, text.c_str(), text.size());
        gtk_entry_set_buffer(_entry, buffer);
    }

    void Entry::set_n_chars(size_t n)
    {
        gtk_entry_set_max_length(_entry, n);
    }
}
