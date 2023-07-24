//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/toggle_button.hpp>
#include <mousetrap/action.hpp>
#include <mousetrap/log.hpp>

namespace mousetrap
{
    ToggleButton::ToggleButton()
        : Widget(gtk_toggle_button_new()),
          CTOR_SIGNAL(ToggleButton, toggled),
          CTOR_SIGNAL(ToggleButton, clicked),
          CTOR_SIGNAL(ToggleButton, realize),
          CTOR_SIGNAL(ToggleButton, unrealize),
          CTOR_SIGNAL(ToggleButton, destroy),
          CTOR_SIGNAL(ToggleButton, hide),
          CTOR_SIGNAL(ToggleButton, show),
          CTOR_SIGNAL(ToggleButton, map),
          CTOR_SIGNAL(ToggleButton, unmap)
    {
        gtk_widget_class_set_activate_signal_from_name(GTK_WIDGET_GET_CLASS(Widget::operator NativeWidget()), "toggled");
        _internal = g_object_ref(GTK_TOGGLE_BUTTON(Widget::operator NativeWidget()));
    }
    
    ToggleButton::ToggleButton(detail::ToggleButtonInternal* internal)
        : Widget(GTK_WIDGET(internal)),
          CTOR_SIGNAL(ToggleButton, toggled),
          CTOR_SIGNAL(ToggleButton, clicked),
          CTOR_SIGNAL(ToggleButton, realize),
          CTOR_SIGNAL(ToggleButton, unrealize),
          CTOR_SIGNAL(ToggleButton, destroy),
          CTOR_SIGNAL(ToggleButton, hide),
          CTOR_SIGNAL(ToggleButton, show),
          CTOR_SIGNAL(ToggleButton, map),
          CTOR_SIGNAL(ToggleButton, unmap)
    {
        _internal = g_object_ref(internal);
    }
    
    ToggleButton::~ToggleButton() 
    {
        g_object_unref(_internal);
    }

    NativeObject ToggleButton::get_internal() const 
    {
        return G_OBJECT(_internal);
    }

    bool ToggleButton::get_is_active() const
    {
        return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(_internal)) == TRUE;
    }

    void ToggleButton::set_is_active(bool b)
    {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(_internal), b ? TRUE : FALSE);
    }

    void ToggleButton::set_child(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(ToggleButton::set_child, this, ptr);
        gtk_button_set_child(GTK_BUTTON(GTK_TOGGLE_BUTTON(_internal)), widget.operator GtkWidget*());
    }

    void ToggleButton::set_icon(const Icon& icon)
    {
        gtk_button_set_child(GTK_BUTTON(operator NativeWidget()), gtk_image_new_from_gicon(icon.operator GIcon *()));
    }

    void ToggleButton::remove_child()
    {
        gtk_button_set_child(GTK_BUTTON(GTK_TOGGLE_BUTTON(_internal)), nullptr);
    }

    void ToggleButton::set_has_frame(bool b)
    {
        gtk_button_set_has_frame(GTK_BUTTON(GTK_TOGGLE_BUTTON(_internal)), b);
    }

    bool ToggleButton::get_has_frame() const {
        return gtk_button_get_has_frame(GTK_BUTTON(GTK_TOGGLE_BUTTON(_internal)));
    }


    void ToggleButton::set_is_circular(bool b)
    {
        if (b and not get_is_circular())
            gtk_widget_add_css_class(GTK_WIDGET(GTK_TOGGLE_BUTTON(_internal)), "circular");
        else if (not b and get_is_circular())
            gtk_widget_remove_css_class(GTK_WIDGET(GTK_TOGGLE_BUTTON(_internal)), "circular");
    }

    bool ToggleButton::get_is_circular() const
    {
        return gtk_widget_has_css_class(GTK_WIDGET(GTK_TOGGLE_BUTTON(_internal)), "circular");
    }
}