//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <include/overlay.hpp>
#include <include/log.hpp>

namespace mousetrap
{
    Overlay::Overlay()
        : WidgetImplementation<GtkOverlay>(GTK_OVERLAY(gtk_overlay_new()))
    {}

    void Overlay::set_child(const Widget& in)
    {
        auto* ptr = &in;
        WARN_IF_SELF_INSERTION(Overlay::set_child, this, ptr);

        _child = ptr;
        gtk_overlay_set_child(get_native(), in.operator GtkWidget*());
    }

    void Overlay::add_overlay(const Widget& widget, bool included_in_measurement, bool clip)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(Overlay::add_overlay, this, ptr);

        auto* gtk_widget = widget.operator GtkWidget*();
        if (gtk_widget == nullptr)
            return;

        gtk_overlay_add_overlay(get_native(), gtk_widget);
        gtk_overlay_set_measure_overlay(get_native(), gtk_widget, included_in_measurement);
        gtk_overlay_set_clip_overlay(get_native(), gtk_widget, clip);
    }

    void Overlay::remove_overlay(const Widget& in)
    {
        gtk_overlay_remove_overlay(get_native(), in.operator GtkWidget*());
    }

    void Overlay::remove_child()
    {
        gtk_overlay_set_child(get_native(), nullptr);
    }

    Widget* Overlay::get_child() const
    {
        return const_cast<Widget*>(_child);
    }
}