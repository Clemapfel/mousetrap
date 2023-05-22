//
// Copyright 2022 Clemens Cords
// Created on 9/3/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/popover.hpp>
#include <mousetrap/log.hpp>

namespace mousetrap
{
    Popover::Popover()
        : Widget(gtk_popover_new()),
          CTOR_SIGNAL(Popover, closed)
    {}
    
    Popover::Popover(detail::PopoverInternal* internal)
        : Widget(GTK_WIDGET(internal)),
          CTOR_SIGNAL(Popover, closed)
    {
        _internal = g_object_ref(internal);
    }
    
    Popover::~Popover()
    {
        g_object_unref(_internal);
    }

    NativeObject Popover::get_internal() const 
    {
        return G_OBJECT(_internal);
    }

    void Popover::popup()
    {
        gtk_popover_popup(GTK_POPOVER(operator NativeWidget()));
    }

    void Popover::popdown()
    {
        gtk_popover_popdown(GTK_POPOVER(operator NativeWidget()));
    }

    void Popover::present()
    {
        gtk_popover_present(GTK_POPOVER(operator NativeWidget()));
    }

    void Popover::set_child(const Widget& child)
    {
        auto* ptr = &child;
        WARN_IF_SELF_INSERTION(Popover::set_child, this, ptr);
        gtk_popover_set_child(GTK_POPOVER(operator NativeWidget()), child.operator NativeWidget());
    }

    void Popover::remove_child()
    {
        gtk_popover_set_child(GTK_POPOVER(operator NativeWidget()), nullptr);
    }

    void Popover::set_relative_position(RelativePosition position)
    {
        gtk_popover_set_position(GTK_POPOVER(operator NativeWidget()), (GtkPositionType) position);
    }

    RelativePosition Popover::get_relative_position() const
    {
        return (RelativePosition) gtk_popover_get_position(GTK_POPOVER(operator NativeWidget()));
    }

    void Popover::set_has_base_arrow(bool b)
    {
        gtk_popover_set_has_arrow(GTK_POPOVER(operator NativeWidget()), b);
    }

    bool Popover::get_has_base_arrow() const
    {
        return gtk_popover_get_has_arrow(GTK_POPOVER(operator NativeWidget()));
    }

    void Popover::set_autohide(bool b)
    {
        gtk_popover_set_autohide(GTK_POPOVER(operator NativeWidget()), b);
    }

    void Popover::attach_to(const Widget& widget)
    {
        gtk_widget_set_parent(GTK_WIDGET(GTK_POPOVER(operator NativeWidget())), widget.operator GtkWidget*());
    }
}