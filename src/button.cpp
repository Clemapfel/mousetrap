//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/button.hpp>
#include <mousetrap/log.hpp>

namespace mousetrap
{
    Button::Button()
        : Widget(gtk_button_new()),
          CTOR_SIGNAL(Button, activate),
          CTOR_SIGNAL(Button, clicked)
    {
        _internal = GTK_BUTTON(operator NativeWidget());
    }
    
    Button::Button(detail::ButtonInternal* internal) 
        : Widget(GTK_WIDGET(internal)),
          CTOR_SIGNAL(Button, activate),
          CTOR_SIGNAL(Button, clicked)
    {
        _internal = g_object_ref(internal);
    }
    
    Button::~Button() 
    {
        g_object_unref(_internal);
    }

    NativeObject Button::get_internal() const 
    {
        return G_OBJECT(_internal);
    }
   
    void Button::set_is_circular(bool b)
    {
        if (b and not get_is_circular())
            gtk_widget_add_css_class(GTK_WIDGET(operator NativeWidget()), "circular");
        else if (not b and get_is_circular())
            gtk_widget_remove_css_class(GTK_WIDGET(operator NativeWidget()), "circular");
    }

    bool Button::get_is_circular() const
    {
        return gtk_widget_has_css_class(GTK_WIDGET(operator NativeWidget()), "circular");
    }

    void Button::set_has_frame(bool b)
    {
        gtk_button_set_has_frame(GTK_BUTTON(operator NativeWidget()), b);
    }

    bool Button::get_has_frame() const
    {
        return gtk_button_get_has_frame(GTK_BUTTON(operator NativeWidget()));
    }

    void Button::set_child(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(Button::push_back, this, ptr);
        gtk_button_set_child(GTK_BUTTON(operator NativeWidget()), widget.operator GtkWidget*());
    }

    void Button::remove_child()
    {
        gtk_button_set_child(GTK_BUTTON(operator NativeWidget()), nullptr);
    }

    void Button::set_action(const Action& action)
    {
        gtk_actionable_set_action_name(GTK_ACTIONABLE(operator NativeWidget()), ("app."+ action.get_id()).c_str());
    }
}