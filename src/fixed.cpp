//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/18/23
//

#include <mousetrap/fixed.hpp>
#include <mousetrap/log.hpp>

namespace mousetrap
{
    Fixed::Fixed()
        : Widget(gtk_fixed_new()),
          CTOR_SIGNAL(Fixed, realize),
          CTOR_SIGNAL(Fixed, unrealize),
          CTOR_SIGNAL(Fixed, destroy),
          CTOR_SIGNAL(Fixed, hide),
          CTOR_SIGNAL(Fixed, show),
          CTOR_SIGNAL(Fixed, map),
          CTOR_SIGNAL(Fixed, unmap)
    {
        _internal = g_object_ref_sink(GTK_FIXED(Widget::operator NativeWidget()));
    }
    
    Fixed::Fixed(detail::FixedInternal* internal) 
        : Widget(GTK_WIDGET(internal)),
          CTOR_SIGNAL(Fixed, realize),
          CTOR_SIGNAL(Fixed, unrealize),
          CTOR_SIGNAL(Fixed, destroy),
          CTOR_SIGNAL(Fixed, hide),
          CTOR_SIGNAL(Fixed, show),
          CTOR_SIGNAL(Fixed, map),
          CTOR_SIGNAL(Fixed, unmap)
    {
        _internal = g_object_ref(internal);
    }
    
    Fixed::~Fixed()
    {
        g_object_unref(_internal);
    }

    NativeObject Fixed::get_internal() const 
    {
        return G_OBJECT(_internal);
    }

    void Fixed::add_child(const Widget& widget, Vector2f pos)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(Fixed::add_child, this, ptr);
        WARN_IF_PARENT_EXISTS(Fixed::add_child, widget);

        gtk_fixed_put(GTK_FIXED(operator NativeWidget()), widget.operator GtkWidget *(), pos.x, pos.y);
    }

    void Fixed::remove_child(const Widget& widget)
    {
        gtk_fixed_remove(GTK_FIXED(operator NativeWidget()), widget.operator GtkWidget*());
    }

    void Fixed::set_child_position(const Widget& widget, Vector2f pos)
    {
        gtk_fixed_move(GTK_FIXED(operator NativeWidget()), widget.operator GtkWidget*(), pos.x, pos.y);
    }

    /*
    Vector2f Fixed::get_child_position(const Widget& widget)
    {
        double x, y;
        gtk_fixed_get_child_position(GTK_FIXED(operator NativeWidget()), widget.operator GtkWidget*(), &x, &y);
        return Vector2f(x, y);
    }
     */
}