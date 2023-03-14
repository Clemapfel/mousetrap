// 
// Copyright 2022 Clemens Cords
// Created on 9/18/22 by clem (mail@clemens-cords.com)
//

#include <include/menu_button.hpp>

namespace mousetrap
{
    MenuButton::MenuButton()
        : WidgetImplementation<GtkMenuButton>(GTK_MENU_BUTTON(gtk_menu_button_new()))
    {
        gtk_menu_button_set_always_show_arrow(get_native(), true);
    }

    void MenuButton::set_child(Widget* widget)
    {
        gtk_menu_button_set_child(get_native(), widget == nullptr ? nullptr : widget->operator GtkWidget*());
    }

    void MenuButton::set_model(MenuModel* model)
    {
        gtk_menu_button_set_menu_model(get_native(), model->operator GMenuModel*());
    }

    void MenuButton::set_popover_position(GtkPositionType type)
    {
        gtk_popover_set_position(gtk_menu_button_get_popover(get_native()), type);
    }

    void MenuButton::set_popover(Popover* popover)
    {
        gtk_menu_button_set_popover(get_native(), popover->operator GtkWidget*());
    }

    void MenuButton::set_popover(PopoverMenu* popover_menu)
    {
        gtk_menu_button_set_popover(get_native(), popover_menu->operator GtkWidget*());
    }

    void MenuButton::popup()
    {
        gtk_menu_button_popup(get_native());
    }

    void MenuButton::popdown()
    {
        gtk_menu_button_popdown(get_native());
    }

    void MenuButton::set_always_show_arrow(bool b)
    {
        gtk_menu_button_set_always_show_arrow(get_native(), b);
    }

    void MenuButton::set_has_frame(bool b)
    {
        gtk_menu_button_set_has_frame(get_native(), b);
    }
}