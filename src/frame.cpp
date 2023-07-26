//
// Copyright 2022 Clemens Cords
// Created on 8/9/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/frame.hpp>
#include <mousetrap/log.hpp>

namespace mousetrap
{
    Frame::Frame()
        : Widget(gtk_frame_new("")),
          CTOR_SIGNAL(Frame, realize),
          CTOR_SIGNAL(Frame, unrealize),
          CTOR_SIGNAL(Frame, destroy),
          CTOR_SIGNAL(Frame, hide),
          CTOR_SIGNAL(Frame, show),
          CTOR_SIGNAL(Frame, map),
          CTOR_SIGNAL(Frame, unmap)
    {
        _internal = g_object_ref_sink(GTK_FRAME(Widget::operator NativeWidget()));
        remove_label_widget();
    }
    
    Frame::Frame(detail::FrameInternal* internal) 
        : Widget(GTK_WIDGET(internal)),
          CTOR_SIGNAL(Frame, realize),
          CTOR_SIGNAL(Frame, unrealize),
          CTOR_SIGNAL(Frame, destroy),
          CTOR_SIGNAL(Frame, hide),
          CTOR_SIGNAL(Frame, show),
          CTOR_SIGNAL(Frame, map),
          CTOR_SIGNAL(Frame, unmap)
    {
        _internal = g_object_ref(internal);
    }
    
    Frame::~Frame()
    {
        g_object_unref(_internal);
    }

    NativeObject Frame::get_internal() const 
    {
        return G_OBJECT(_internal);
    }

    void Frame::set_child(const Widget& in)
    {
        auto ptr = &in;
        WARN_IF_SELF_INSERTION(Frame::set_child, this, ptr);
        WARN_IF_PARENT_EXISTS(Frame::set_child, in);

        gtk_frame_set_child(GTK_FRAME(operator NativeWidget()), in.operator GtkWidget *());
    }

    void Frame::remove_child()
    {
        gtk_frame_set_child(GTK_FRAME(operator NativeWidget()), nullptr);
    }

    void Frame::set_label_widget(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(Frame::set_label_widget, this, ptr);
        WARN_IF_PARENT_EXISTS(Frame::set_label_widget, widget);

        gtk_frame_set_label_widget(GTK_FRAME(operator NativeWidget()), widget.operator GtkWidget*());
    }

    void Frame::remove_label_widget()
    {
        gtk_frame_set_label_widget(GTK_FRAME(operator NativeWidget()), nullptr);
    }

    void Frame::set_label_x_alignment(float x)
    {
        gtk_frame_set_label_align(GTK_FRAME(operator NativeWidget()), x);
    }

    float Frame::get_label_x_alignment() const
    {
        return gtk_frame_get_label_align(GTK_FRAME(operator NativeWidget()));
    }
}