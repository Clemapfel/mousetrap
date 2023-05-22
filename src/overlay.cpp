//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/overlay.hpp>
#include <mousetrap/log.hpp>

namespace mousetrap
{
    Overlay::Overlay()
        : Widget(gtk_overlay_new())
    {
        _internal = g_object_ref_sink(GTK_OVERLAY(Widget::operator NativeWidget()));
    }
    
    Overlay::Overlay(detail::OverlayInternal* internal)
        : Widget(GTK_WIDGET(internal))
    {
        g_object_ref(internal);
    }
    
    Overlay::~Overlay() 
    {
        g_object_unref(_internal);
    }

    NativeObject Overlay::get_internal() const 
    {
        return G_OBJECT(_internal);
    }

    void Overlay::set_child(const Widget& in)
    {
        auto* ptr = &in;
        WARN_IF_SELF_INSERTION(Overlay::set_child, this, ptr);

        gtk_overlay_set_child(GTK_OVERLAY(operator NativeWidget()), in.operator GtkWidget*());
    }

    void Overlay::add_overlay(const Widget& widget, bool included_in_measurement, bool clip)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(Overlay::add_overlay, this, ptr);

        auto* gtk_widget = widget.operator GtkWidget*();
        gtk_overlay_add_overlay(GTK_OVERLAY(operator NativeWidget()), gtk_widget);
        gtk_overlay_set_measure_overlay(GTK_OVERLAY(operator NativeWidget()), gtk_widget, included_in_measurement);
        gtk_overlay_set_clip_overlay(GTK_OVERLAY(operator NativeWidget()), gtk_widget, clip);
    }

    void Overlay::remove_overlay(const Widget& in)
    {
        gtk_overlay_remove_overlay(GTK_OVERLAY(operator NativeWidget()), in.operator GtkWidget*());
    }

    void Overlay::remove_child()
    {
        gtk_overlay_set_child(GTK_OVERLAY(operator NativeWidget()), nullptr);
    }
}