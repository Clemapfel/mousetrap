//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <include/label.hpp>

namespace mousetrap
{
    Label::Label()
    : Label("")
    {}

    Label::Label(const std::string& str)
    : WidgetImplementation<GtkLabel>(GTK_LABEL(gtk_label_new(str.c_str())))
    {
        gtk_label_set_use_markup(get_native(), true);
    }

    void Label::set_text(const std::string& text)
    {
        bool use_markup = gtk_label_get_use_markup(get_native());
        gtk_label_set_text(get_native(), text.c_str());
        gtk_label_set_use_markup(get_native(), use_markup);
    }

    std::string Label::get_text()
    {
        return std::string(gtk_label_get_text(get_native()));
    }

    void Label::set_use_markup(bool b)
    {
        gtk_label_set_use_markup(get_native(), b);
    }

    bool Label::get_use_markup() const
    {
        return gtk_label_get_use_markup(get_native());
    }

    void Label::set_ellipsize_mode(EllipsizeMode mode)
    {
        gtk_label_set_ellipsize(get_native(), (PangoEllipsizeMode) mode);
    }

    void Label::set_wrap_mode(LabelWrapMode mode)
    {
        if (mode == LabelWrapMode::NONE)
        {
            gtk_label_set_wrap(get_native(), false);
            gtk_label_set_wrap_mode(get_native(), PANGO_WRAP_CHAR);
            gtk_label_set_lines(get_native(), 1);
            gtk_label_set_single_line_mode(get_native(), true);
        }
        else
        {
            gtk_label_set_wrap(get_native(), true);
            gtk_label_set_wrap_mode(get_native(), (PangoWrapMode) mode);
            gtk_label_set_lines(get_native(), -1);
            gtk_label_set_single_line_mode(get_native(), false);
        }
    }

    LabelWrapMode Label::get_wrap_mode() const
    {
        if (gtk_label_get_wrap(get_native()) == false)
            return LabelWrapMode::NONE;
        else
            return (LabelWrapMode) gtk_label_get_wrap_mode(get_native());
    }

    void Label::set_justify_mode(JustifyMode mode)
    {
        gtk_label_set_justify(get_native(), (GtkJustification) mode);
    }

    JustifyMode Label::get_justify_mode() const
    {
        return (JustifyMode) gtk_label_get_justify(get_native());
    }

    void Label::set_max_width_chars(size_t n_chars)
    {
        gtk_label_set_max_width_chars(get_native(), n_chars);
    }

    size_t Label::get_max_width_chars() const
    {
        return gtk_label_get_max_width_chars(get_native());
    }

    void Label::set_x_alignment(float x)
    {
        gtk_label_set_xalign(get_native(), x);
    }

    float Label::get_x_alignment() const
    {
        return gtk_label_get_xalign(get_native());
    }

    void Label::set_y_alignment(float y)
    {
        return gtk_label_set_yalign(get_native(), y);
    }

    float Label::get_y_alignment() const
    {
        return gtk_label_get_yalign(get_native());
    }

    void Label::set_selectable(bool b)
    {
        gtk_label_set_selectable(get_native(), b);
    }

    bool Label::get_selectable() const
    {
        return gtk_label_get_selectable(get_native());
    }
}