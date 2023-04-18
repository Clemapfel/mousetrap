//
// Created by clem on 3/19/23.
//

#include <include/text_view.hpp>

namespace mousetrap
{
    TextView::TextView()
        : WidgetImplementation<GtkTextView>(GTK_TEXT_VIEW(gtk_text_view_new())),
          CTOR_SIGNAL(TextView, text_changed),
          CTOR_SIGNAL(TextView, undo),
          CTOR_SIGNAL(TextView, redo)
    {}

    std::string TextView::get_text() const
    {
        auto* buffer = gtk_text_view_get_buffer(get_native());
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        return std::string(gtk_text_buffer_get_text(buffer, &start, &end, false));
    }

    void TextView::set_text(const std::string& text) const
    {
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(get_native()), text.c_str(), text.size());
    }

    void TextView::set_cursor_visible(bool b)
    {
        gtk_text_view_set_cursor_visible(get_native(), b);
    }

    bool TextView::get_cursor_visible() const
    {
        return gtk_text_view_get_cursor_visible(get_native());
    }

    void TextView::undo()
    {
        gtk_text_buffer_undo(gtk_text_view_get_buffer(get_native()));
    }

    void TextView::redo()
    {
        gtk_text_buffer_redo(gtk_text_view_get_buffer(get_native()));
    }

    void TextView::set_was_modified(bool b)
    {
        gtk_text_buffer_set_modified(gtk_text_view_get_buffer(get_native()), b);
    }

    bool TextView::get_was_modified() const
    {
        return gtk_text_buffer_get_modified(gtk_text_view_get_buffer(get_native()));
    }

    void TextView::set_editable(bool b)
    {
        gtk_text_view_set_editable(get_native(), b);
    }

    bool TextView::get_editable() const
    {
        return gtk_text_view_get_editable(get_native());
    }

    void TextView::set_justify_mode(JustifyMode mode)
    {
        gtk_text_view_set_justification(get_native(), (GtkJustification) mode);
    }

    JustifyMode TextView::get_justify_mode() const
    {
        return (JustifyMode) gtk_text_view_get_justification(get_native());
    }

    void TextView::set_left_margin(float v)
    {
        gtk_text_view_set_left_margin(get_native(), v);
    }

    float TextView::get_left_margin() const
    {
        return gtk_text_view_get_left_margin(get_native());
    }

    void TextView::set_right_margin(float v)
    {
        gtk_text_view_set_right_margin(get_native(), v);
    }

    float TextView::get_right_margin() const
    {
        return gtk_text_view_get_right_margin(get_native());
    }

    void TextView::set_top_margin(float v)
    {
        gtk_text_view_set_top_margin(get_native(), v);
    }

    float TextView::get_top_margin() const
    {
        return gtk_text_view_get_top_margin(get_native());
    }

    void TextView::set_bottom_margin(float v)
    {
        gtk_text_view_set_bottom_margin(get_native(), v);
    }

    float TextView::get_bottom_margin() const
    {
        return gtk_text_view_get_bottom_margin(get_native());
    }
}