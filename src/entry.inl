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

        connect_signal("activate", on_activate_wrapper, this);
        connect_signal("paste-clipboard", on_paste_clipboard_wrapper, this);
        connect_signal("copy-clipboard", on_copy_clipboard_wrapper, this);
        connect_signal("cut-clipboard", on_cut_clipboard_wrapper, this);
    }

    Entry::~Entry()
    {
        //gtk_widget_destroy(GTK_WIDGET(_entry));
        //gtk_widget_destroy(GTK_WIDGET(_buffer));
    };

    GtkWidget* Entry::get_native()
    {
        return GTK_WIDGET(_entry);
    }

    std::string Entry::get_text() const
    {
        return gtk_entry_get_text(_entry);
    }

    void Entry::on_activate(GtkEntry* self, gpointer user_data)
    {
        // noop
    }

    void Entry::on_paste_clipboard(GtkEntry* self, gpointer user_data)
    {
        // noop
    }

    void Entry::on_copy_clipboard(GtkEntry* self, gpointer user_data)
    {
        // noop
    }

    void Entry::on_cut_clipboard(GtkEntry* self, gpointer user_data)
    {
        // noop
    }

    void Entry::on_activate_wrapper(GtkEntry* self, void* instance)
    {
        ((Entry*) instance)->on_activate(self, nullptr);
    }

    void Entry::on_paste_clipboard_wrapper(GtkEntry* self, void* instance)
    {
        ((Entry*) instance)->on_paste_clipboard(self, nullptr);
    }

    void Entry::on_cut_clipboard_wrapper(GtkEntry* self, void* instance)
    {
        ((Entry*) instance)->on_cut_clipboard(self, nullptr);
    }

    void Entry::on_copy_clipboard_wrapper(GtkEntry* self, void* instance)
    {
        ((Entry*) instance)->on_copy_clipboard(self, nullptr);
    }

    void Entry::set_width_chars(size_t n)
    {
        gtk_entry_set_width_chars(_entry, n);
    }
}
