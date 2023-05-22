//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/entry.hpp>

namespace mousetrap
{
    Entry::Entry()
        : Widget(gtk_entry_new()),
          CTOR_SIGNAL(Entry, activate),
          CTOR_SIGNAL(Entry, text_changed)
    {
        _internal = GTK_ENTRY(Widget::operator NativeWidget());
    }
    
    Entry::Entry(detail::EntryInternal* internal)
        : Widget(gtk_entry_new()),
          CTOR_SIGNAL(Entry, activate),
          CTOR_SIGNAL(Entry, text_changed)
    {
        _internal = g_object_ref(internal);
    }
    
    Entry::~Entry()
    {
        g_object_unref(_internal);
    }

    NativeObject Entry::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    std::string Entry::get_text() const
    {
        auto* buffer = gtk_entry_get_buffer(GTK_ENTRY(operator NativeWidget()));
        return gtk_entry_buffer_get_text(buffer);
    }

    void Entry::set_text(const std::string& text)
    {
        auto* buffer = gtk_entry_get_buffer(GTK_ENTRY(operator NativeWidget()));
        gtk_entry_buffer_set_text(buffer, text.c_str(), text.size());
        gtk_entry_buffer_emit_inserted_text(buffer, 0, text.c_str(), text.size());
        gtk_entry_set_buffer(GTK_ENTRY(operator NativeWidget()), buffer);
    }

    void Entry::set_has_frame(bool b)
    {
        gtk_entry_set_has_frame(GTK_ENTRY(operator NativeWidget()), b);
    }

    bool Entry::get_has_frame() const
    {
        return gtk_entry_get_has_frame(GTK_ENTRY(operator NativeWidget()));
    }

    void Entry::set_max_length(size_t n)
    {
        gtk_entry_set_max_length(GTK_ENTRY(operator NativeWidget()), n);
    }

    int Entry::get_max_length() const
    {
        return gtk_entry_get_max_length(GTK_ENTRY(operator NativeWidget()));
    }

    void Entry::set_text_visible(bool b)
    {
        gtk_entry_set_visibility(GTK_ENTRY(operator NativeWidget()), b);
    }

    bool Entry::get_text_visible() const
    {
        return gtk_entry_get_visibility(GTK_ENTRY(operator NativeWidget()));
    }

    void Entry::set_primary_icon(const Icon& icon)
    {
        gtk_entry_set_icon_from_gicon(GTK_ENTRY(operator NativeWidget()), GTK_ENTRY_ICON_PRIMARY, icon.operator GIcon*());
    }

    void Entry::remove_primary_icon()
    {
        gtk_entry_set_icon_from_gicon(GTK_ENTRY(operator NativeWidget()), GTK_ENTRY_ICON_PRIMARY, nullptr);
    }

    void Entry::set_secondary_icon(const Icon& icon)
    {
        gtk_entry_set_icon_from_gicon(GTK_ENTRY(operator NativeWidget()), GTK_ENTRY_ICON_SECONDARY, icon.operator GIcon*());
    }

    void Entry::remove_secondary_icon()
    {
        gtk_entry_set_icon_from_gicon(GTK_ENTRY(operator NativeWidget()), GTK_ENTRY_ICON_SECONDARY, nullptr);
    }
}
