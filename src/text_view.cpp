//
// Created by clem on 3/19/23.
//

#include <mousetrap/text_view.hpp>

namespace mousetrap
{
    TextView::TextView()
        : Widget(gtk_text_view_new()),
          CTOR_SIGNAL(TextView, text_changed),
          // CTOR_SIGNAL(TextView, undo),
          // CTOR_SIGNAL(TextView, redo),
          CTOR_SIGNAL(TextView, realize),
          CTOR_SIGNAL(TextView, unrealize),
          CTOR_SIGNAL(TextView, destroy),
          CTOR_SIGNAL(TextView, hide),
          CTOR_SIGNAL(TextView, show),
          CTOR_SIGNAL(TextView, map),
          CTOR_SIGNAL(TextView, unmap)
    {
        _internal = g_object_ref(GTK_TEXT_VIEW(TextView::operator NativeWidget()));
    }
    
    TextView::TextView(detail::TextViewInternal* internal)
        : Widget(GTK_WIDGET(internal)),
          CTOR_SIGNAL(TextView, text_changed),
          // CTOR_SIGNAL(TextView, undo),
          // CTOR_SIGNAL(TextView, redo),
          CTOR_SIGNAL(TextView, realize),
          CTOR_SIGNAL(TextView, unrealize),
          CTOR_SIGNAL(TextView, destroy),
          CTOR_SIGNAL(TextView, hide),
          CTOR_SIGNAL(TextView, show),
          CTOR_SIGNAL(TextView, map),
          CTOR_SIGNAL(TextView, unmap)
    {
        _internal = g_object_ref(internal);
    }
    
    TextView::~TextView() 
    {
        g_object_unref(_internal);
    }

    NativeObject TextView::get_internal() const 
    {
        return G_OBJECT(_internal);
    }

    TextView::operator NativeObject() const
    {
        return G_OBJECT(gtk_text_view_get_buffer(_internal));
    }

    std::string TextView::get_text() const
    {
        auto* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(_internal));
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        return std::string(gtk_text_buffer_get_text(buffer, &start, &end, false));
    }

    void TextView::set_text(const std::string& text) const
    {
        auto* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(_internal));
        gtk_text_buffer_set_text(buffer, text.c_str(), text.size());
        gtk_text_buffer_set_modified(buffer, true);
    }

    void TextView::set_cursor_visible(bool b)
    {
        gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(_internal), b);
    }

    bool TextView::get_cursor_visible() const
    {
        return gtk_text_view_get_cursor_visible(GTK_TEXT_VIEW(_internal));
    }

    void TextView::undo()
    {
        gtk_text_buffer_undo(gtk_text_view_get_buffer(GTK_TEXT_VIEW(_internal)));
    }

    void TextView::redo()
    {
        gtk_text_buffer_redo(gtk_text_view_get_buffer(GTK_TEXT_VIEW(_internal)));
    }

    void TextView::set_was_modified(bool b)
    {
        gtk_text_buffer_set_modified(gtk_text_view_get_buffer(GTK_TEXT_VIEW(_internal)), b);
    }

    bool TextView::get_was_modified() const
    {
        return gtk_text_buffer_get_modified(gtk_text_view_get_buffer(GTK_TEXT_VIEW(_internal)));
    }

    void TextView::set_editable(bool b)
    {
        gtk_text_view_set_editable(GTK_TEXT_VIEW(_internal), b);
    }

    bool TextView::get_editable() const
    {
        return gtk_text_view_get_editable(GTK_TEXT_VIEW(_internal));
    }

    void TextView::set_justify_mode(JustifyMode mode)
    {
        gtk_text_view_set_justification(GTK_TEXT_VIEW(_internal), (GtkJustification) mode);
    }

    JustifyMode TextView::get_justify_mode() const
    {
        return (JustifyMode) gtk_text_view_get_justification(GTK_TEXT_VIEW(_internal));
    }

    void TextView::set_left_margin(float v)
    {
        gtk_text_view_set_left_margin(GTK_TEXT_VIEW(_internal), v);
    }

    float TextView::get_left_margin() const
    {
        return gtk_text_view_get_left_margin(GTK_TEXT_VIEW(_internal));
    }

    void TextView::set_right_margin(float v)
    {
        gtk_text_view_set_right_margin(GTK_TEXT_VIEW(_internal), v);
    }

    float TextView::get_right_margin() const
    {
        return gtk_text_view_get_right_margin(GTK_TEXT_VIEW(_internal));
    }

    void TextView::set_top_margin(float v)
    {
        gtk_text_view_set_top_margin(GTK_TEXT_VIEW(_internal), v);
    }

    float TextView::get_top_margin() const
    {
        return gtk_text_view_get_top_margin(GTK_TEXT_VIEW(_internal));
    }

    void TextView::set_bottom_margin(float v)
    {
        gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(_internal), v);
    }

    float TextView::get_bottom_margin() const
    {
        return gtk_text_view_get_bottom_margin(GTK_TEXT_VIEW(_internal));
    }
}