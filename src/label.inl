// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

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
        }
        else
        {
            gtk_label_set_wrap(get_native(), true);
            gtk_label_set_wrap_mode(get_native(), (PangoWrapMode) mode);
            gtk_label_set_lines(get_native(), -1);
        }
    }

    void Label::set_justify_mode(JustifyMode mode)
    {
        gtk_label_set_justify(get_native(), (GtkJustification) mode);
    }

    void Label::set_max_width_chars(size_t n_chars)
    {
        gtk_label_set_max_width_chars(get_native(), n_chars);
    }
}