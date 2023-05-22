// 
// Copyright 2022 Clemens Cords
// Created on 9/18/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/popover_button.hpp>
#include <mousetrap/log.hpp>

#include <iostream>

namespace mousetrap
{
    PopoverButton::PopoverButton()
        : Widget(gtk_menu_button_new()),
          CTOR_SIGNAL(PopoverButton, activate)
    {
        gtk_menu_button_set_always_show_arrow(GTK_MENU_BUTTON(operator NativeWidget()), true);
    }

    PopoverButton::PopoverButton(detail::PopoverButtonInternal* internal) 
        : Widget(GTK_WIDGET(internal)),
          CTOR_SIGNAL(PopoverButton, activate)
    {
        _internal = g_object_ref(internal);
    }
    
    PopoverButton::~PopoverButton() 
    {
        g_object_unref(_internal);
    }

    NativeObject PopoverButton::get_internal() const 
    {
        g_object_unref(_internal);
    }
    
    void PopoverButton::set_child(const Widget& child)
    {
        auto* ptr = &child;
        WARN_IF_SELF_INSERTION(PopoverButton::set_child, this, ptr);

        gtk_menu_button_set_child(GTK_MENU_BUTTON(operator NativeWidget()), child.operator GtkWidget*());

        if (_popover_menu != nullptr)
            _popover_menu->refresh_widgets();
    }

    void PopoverButton::remove_child()
    {
        _child = nullptr;
        gtk_menu_button_set_child(GTK_MENU_BUTTON(operator NativeWidget()), nullptr);
    }

    Widget* PopoverButton::get_child() const
    {
        return const_cast<Widget*>(_child);
    }

    void PopoverButton::set_popover_position(RelativePosition type)
    {
        gtk_popover_set_position(gtk_menu_button_get_popover(GTK_MENU_BUTTON(operator NativeWidget())), (GtkPositionType) type);
    }

    RelativePosition PopoverButton::get_popover_position() const
    {
        return (RelativePosition) gtk_popover_get_position(gtk_menu_button_get_popover(GTK_MENU_BUTTON(operator NativeWidget())));
    }

    void PopoverButton::set_popover(Popover& popover)
    {
        _popover = &popover;
        _popover_menu = nullptr;
        gtk_menu_button_set_popover(GTK_MENU_BUTTON(operator NativeWidget()), popover.operator GtkWidget*());
    }

    void PopoverButton::set_popover_menu(PopoverMenu& popover_menu)
    {
        _popover = nullptr;
        _popover_menu = &popover_menu;

        gtk_menu_button_set_popover(GTK_MENU_BUTTON(operator NativeWidget()), popover_menu.operator GtkWidget*());
        _popover_menu->refresh_widgets();
    }

    void PopoverButton::remove_popover()
    {
        _popover_menu = nullptr;
        gtk_menu_button_set_popover(GTK_MENU_BUTTON(operator NativeWidget()), nullptr);
    }

    void PopoverButton::popup()
    {
        gtk_menu_button_popup(GTK_MENU_BUTTON(operator NativeWidget()));
    }

    void PopoverButton::popdown()
    {
        gtk_menu_button_popdown(GTK_MENU_BUTTON(operator NativeWidget()));
    }

    void PopoverButton::set_always_show_arrow(bool b)
    {
        gtk_menu_button_set_always_show_arrow(GTK_MENU_BUTTON(operator NativeWidget()), b);
    }

    void PopoverButton::set_has_frame(bool b)
    {
        gtk_menu_button_set_has_frame(GTK_MENU_BUTTON(operator NativeWidget()), b);
    }

    void PopoverButton::set_is_circular(bool b)
    {
        if (b and not get_is_circular())
            gtk_widget_add_css_class(GTK_WIDGET(GTK_MENU_BUTTON(operator NativeWidget())), "circular");
        else if (not b and get_is_circular())
            gtk_widget_remove_css_class(GTK_WIDGET(GTK_MENU_BUTTON(operator NativeWidget())), "circular");
    }

    bool PopoverButton::get_is_circular() const
    {
        return gtk_widget_has_css_class(GTK_WIDGET(GTK_MENU_BUTTON(operator NativeWidget())), "circular");
    }
}