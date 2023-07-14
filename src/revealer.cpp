//
// Copyright 2022 Clemens Cords
// Created on 9/13/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/revealer.hpp>
#include <mousetrap/log.hpp>

namespace mousetrap
{
    Revealer::Revealer(RevealerTransitionType type)
        : Widget(gtk_revealer_new()),
          CTOR_SIGNAL(Revealer, revealed),
          CTOR_SIGNAL(Revealer, realize),
          CTOR_SIGNAL(Revealer, unrealize),
          CTOR_SIGNAL(Revealer, destroy),
          CTOR_SIGNAL(Revealer, hide),
          CTOR_SIGNAL(Revealer, show),
          CTOR_SIGNAL(Revealer, map),
          CTOR_SIGNAL(Revealer, unmap)
    {
        _internal = g_object_ref(GTK_REVEALER(Widget::operator NativeWidget()));

        gtk_revealer_set_reveal_child(GTK_REVEALER(operator NativeWidget()), true);
        set_transition_type(type);
    }
    
    Revealer::Revealer(detail::RevealerInternal* internal)
        : Widget(GTK_WIDGET(internal)),
          CTOR_SIGNAL(Revealer, revealed),
          CTOR_SIGNAL(Revealer, realize),
          CTOR_SIGNAL(Revealer, unrealize),
          CTOR_SIGNAL(Revealer, destroy),
          CTOR_SIGNAL(Revealer, hide),
          CTOR_SIGNAL(Revealer, show),
          CTOR_SIGNAL(Revealer, map),
          CTOR_SIGNAL(Revealer, unmap)
    {
        _internal = g_object_ref(internal);
    }
    
    Revealer::~Revealer() 
    {
        g_object_unref(_internal);
    }

    NativeObject Revealer::get_internal() const 
    {
        return G_OBJECT(_internal);
    }

    void Revealer::set_child(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(Revealer::set_child, this, ptr);

        gtk_revealer_set_child(GTK_REVEALER(operator NativeWidget()), widget.operator GtkWidget*());
    }

    void Revealer::remove_child()
    {
        gtk_revealer_set_child(GTK_REVEALER(operator NativeWidget()), nullptr);
    }

    void Revealer::set_is_revealed(bool b)
    {
        gtk_revealer_set_reveal_child(GTK_REVEALER(operator NativeWidget()), b);

        // trigger parent container reorder
        auto* child = gtk_revealer_get_child(GTK_REVEALER(operator NativeWidget()));
        auto h = gtk_widget_get_hexpand(child);
        auto v = gtk_widget_get_vexpand(child);
        gtk_widget_set_hexpand(child, not h);
        gtk_widget_set_vexpand(child, not v);
        gtk_widget_set_hexpand(child, h);
        gtk_widget_set_vexpand(child, v);
    }

    bool Revealer::get_is_revealed() const
    {
        return gtk_revealer_get_reveal_child(GTK_REVEALER(operator NativeWidget()));
    }

    void Revealer::set_transition_type(RevealerTransitionType type)
    {
        gtk_revealer_set_transition_type(GTK_REVEALER(operator NativeWidget()), (GtkRevealerTransitionType) type);
    }

    RevealerTransitionType Revealer::get_transition_type() const
    {
        return (RevealerTransitionType) gtk_revealer_get_transition_type(GTK_REVEALER(operator NativeWidget()));
    }

    void Revealer::set_transition_duration(Time time)
    {
        gtk_revealer_set_transition_duration(GTK_REVEALER(operator NativeWidget()), time.as_milliseconds());
    }

    Time Revealer::get_transition_duration() const
    {
        return milliseconds(gtk_revealer_get_transition_duration(GTK_REVEALER(operator NativeWidget())));
    }
}