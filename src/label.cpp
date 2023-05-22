//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/label.hpp>

namespace mousetrap
{
    Label::Label()
        : Label("")
    {}

    Label::Label(const std::string& str)
        : Widget(gtk_label_new(str.c_str()))
    {
        gtk_label_set_use_markup(GTK_LABEL(Widget::operator NativeWidget()), true);
        _internal = GTK_LABEL(Widget::operator NativeWidget());
        g_object_ref(_internal);
    }
    
    Label::Label(detail::LabelInternal* internal)
        : Widget(GTK_WIDGET(internal))
    {
        _internal = g_object_ref(internal);
    }

    Label::~Label()
    {
        g_object_unref(_internal);
    }

    NativeObject Label::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    void Label::set_text(const std::string& text)
    {
        bool use_markup = gtk_label_get_use_markup(GTK_LABEL(operator NativeWidget()));
        gtk_label_set_text(GTK_LABEL(operator NativeWidget()), text.c_str());
        gtk_label_set_use_markup(GTK_LABEL(operator NativeWidget()), use_markup);
    }

    std::string Label::get_text()
    {
        return std::string(gtk_label_get_text(GTK_LABEL(operator NativeWidget())));
    }

    void Label::set_use_markup(bool b)
    {
        gtk_label_set_use_markup(GTK_LABEL(operator NativeWidget()), b);
    }

    bool Label::get_use_markup() const
    {
        return gtk_label_get_use_markup(GTK_LABEL(operator NativeWidget()));
    }

    void Label::set_ellipsize_mode(EllipsizeMode mode)
    {
        gtk_label_set_ellipsize(GTK_LABEL(operator NativeWidget()), (PangoEllipsizeMode) mode);
    }

    void Label::set_wrap_mode(LabelWrapMode mode)
    {
        if (mode == LabelWrapMode::NONE)
        {
            gtk_label_set_wrap(GTK_LABEL(operator NativeWidget()), false);
            gtk_label_set_wrap_mode(GTK_LABEL(operator NativeWidget()), PANGO_WRAP_CHAR);
            gtk_label_set_lines(GTK_LABEL(operator NativeWidget()), 1);
            gtk_label_set_single_line_mode(GTK_LABEL(operator NativeWidget()), true);
        }
        else
        {
            gtk_label_set_wrap(GTK_LABEL(operator NativeWidget()), true);
            gtk_label_set_wrap_mode(GTK_LABEL(operator NativeWidget()), (PangoWrapMode) mode);
            gtk_label_set_lines(GTK_LABEL(operator NativeWidget()), -1);
            gtk_label_set_single_line_mode(GTK_LABEL(operator NativeWidget()), false);
        }
    }

    LabelWrapMode Label::get_wrap_mode() const
    {
        if (gtk_label_get_wrap(GTK_LABEL(operator NativeWidget())) == false)
            return LabelWrapMode::NONE;
        else
            return (LabelWrapMode) gtk_label_get_wrap_mode(GTK_LABEL(operator NativeWidget()));
    }

    void Label::set_justify_mode(JustifyMode mode)
    {
        gtk_label_set_justify(GTK_LABEL(operator NativeWidget()), (GtkJustification) mode);
    }

    JustifyMode Label::get_justify_mode() const
    {
        return (JustifyMode) gtk_label_get_justify(GTK_LABEL(operator NativeWidget()));
    }

    void Label::set_max_width_chars(size_t n_chars)
    {
        gtk_label_set_max_width_chars(GTK_LABEL(operator NativeWidget()), n_chars);
    }

    size_t Label::get_max_width_chars() const
    {
        return gtk_label_get_max_width_chars(GTK_LABEL(operator NativeWidget()));
    }

    void Label::set_x_alignment(float x)
    {
        gtk_label_set_xalign(GTK_LABEL(operator NativeWidget()), x);
    }

    float Label::get_x_alignment() const
    {
        return gtk_label_get_xalign(GTK_LABEL(operator NativeWidget()));
    }

    void Label::set_y_alignment(float y)
    {
        return gtk_label_set_yalign(GTK_LABEL(operator NativeWidget()), y);
    }

    float Label::get_y_alignment() const
    {
        return gtk_label_get_yalign(GTK_LABEL(operator NativeWidget()));
    }

    void Label::set_selectable(bool b)
    {
        gtk_label_set_selectable(GTK_LABEL(operator NativeWidget()), b);
    }

    bool Label::get_selectable() const
    {
        return gtk_label_get_selectable(GTK_LABEL(operator NativeWidget()));
    }
}