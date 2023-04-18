//
// Copyright 2022 Clemens Cords
// Created on 9/13/22 by clem (mail@clemens-cords.com)
//

#include <include/revealer.hpp>
#include <include/log.hpp>

namespace mousetrap
{
    Revealer::Revealer(RevealerTransitionType type)
    : WidgetImplementation(GTK_REVEALER(gtk_revealer_new()))
    {
        gtk_revealer_set_reveal_child(get_native(), true);
        set_transition_type(type);
    }

    void Revealer::set_child(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(Revealer::set_child, this, ptr);

        gtk_revealer_set_child(get_native(), widget.operator GtkWidget*());
        _child = ptr;
    }

    Widget* Revealer::get_child() const
    {
        return const_cast<Widget*>(_child);
    }

    void Revealer::set_revealed(bool b)
    {
        gtk_revealer_set_reveal_child(get_native(), b);

        // trigger parent container reorder
        auto* child = gtk_revealer_get_child(get_native());
        auto h = gtk_widget_get_hexpand(child);
        auto v = gtk_widget_get_vexpand(child);
        gtk_widget_set_hexpand(child, not h);
        gtk_widget_set_vexpand(child, not v);
        gtk_widget_set_hexpand(child, h);
        gtk_widget_set_vexpand(child, v);
    }

    bool Revealer::get_revealed() const
    {
        return gtk_revealer_get_reveal_child(get_native());
    }

    void Revealer::set_transition_type(RevealerTransitionType type)
    {
        gtk_revealer_set_transition_type(get_native(), (GtkRevealerTransitionType) type);
    }

    RevealerTransitionType Revealer::get_transition_type() const
    {
        return (RevealerTransitionType) gtk_revealer_get_transition_type(get_native());
    }

    void Revealer::set_transition_duration(Time time)
    {
        gtk_revealer_set_transition_duration(get_native(), time.as_milliseconds());
    }

    Time Revealer::get_transition_duration() const
    {
        return milliseconds(gtk_revealer_get_transition_duration(get_native()));
    }
}