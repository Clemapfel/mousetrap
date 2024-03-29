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
          CTOR_SIGNAL(Popover, closed),
          CTOR_SIGNAL(Popover, realize),
          CTOR_SIGNAL(Popover, unrealize),
          CTOR_SIGNAL(Popover, destroy),
          CTOR_SIGNAL(Popover, hide),
          CTOR_SIGNAL(Popover, show),
          CTOR_SIGNAL(Popover, map),
          CTOR_SIGNAL(Popover, unmap)
    {
        _internal = g_object_ref_sink(GTK_POPOVER(Widget::operator NativeWidget()));
    }
    
    Popover::Popover(detail::PopoverInternal* internal)
        : Widget(GTK_WIDGET(internal)),
          CTOR_SIGNAL(Popover, closed),
          CTOR_SIGNAL(Popover, realize),
          CTOR_SIGNAL(Popover, unrealize),
          CTOR_SIGNAL(Popover, destroy),
          CTOR_SIGNAL(Popover, hide),
          CTOR_SIGNAL(Popover, show),
          CTOR_SIGNAL(Popover, map),
          CTOR_SIGNAL(Popover, unmap)
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
        if (gtk_widget_get_parent(operator NativeWidget()) == nullptr)
            log::critical("In Popover::popup: Popover cannot be shown because it does not have a parent.", MOUSETRAP_DOMAIN);
        else
            gtk_popover_popup(GTK_POPOVER(operator NativeWidget()));
    }

    void Popover::popdown()
    {
        gtk_popover_popdown(GTK_POPOVER(operator NativeWidget()));
    }

    void Popover::set_child(const Widget& child)
    {
        auto* ptr = &child;
        WARN_IF_SELF_INSERTION(Popover::set_child, this, ptr);
        WARN_IF_PARENT_EXISTS(Popover::set_child, child);

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

    bool Popover::get_autohide() const
    {
        return gtk_popover_get_autohide(GTK_POPOVER(operator NativeWidget()));
    }
}