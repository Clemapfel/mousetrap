//
// Copyright 2022 Clemens Cords
// Created on 8/9/22 by clem (mail@clemens-cords.com)
//

#include <include/frame.hpp>
#include <include/log.hpp>

namespace mousetrap
{
    Frame::Frame()
        : WidgetImplementation<GtkFrame>(GTK_FRAME(gtk_frame_new("")))
    {
        remove_label_widget();
    }

    void Frame::set_child(const Widget& in)
    {
        auto ptr = &in;
        WARN_IF_SELF_INSERTION(Frame::set_child, this, ptr);

        _child = ptr;
        gtk_frame_set_child(get_native(), in.operator GtkWidget *());
    }

    Widget* Frame::get_child() const
    {
        return const_cast<Widget*>(_child);
    }

    void Frame::remove_child()
    {
        gtk_frame_set_child(get_native(), nullptr);
    }

    void Frame::set_label_widget(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(Frame::set_label_widget, this, ptr);

        _label_widget = ptr;
        gtk_frame_set_label_widget(get_native(), widget.operator GtkWidget*());
    }

    Widget* Frame::get_label_widget() const
    {
        return const_cast<Widget*>(_label_widget);
    }

    void Frame::remove_label_widget()
    {
        gtk_frame_set_label_widget(get_native(), nullptr);
    }

    void Frame::set_label_x_alignment(float x)
    {
        gtk_frame_set_label_align(get_native(), x);
    }

    float Frame::get_label_x_alignment() const
    {
        return gtk_frame_get_label_align(get_native());
    }
}