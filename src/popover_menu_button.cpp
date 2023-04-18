// 
// Copyright 2022 Clemens Cords
// Created on 9/18/22 by clem (mail@clemens-cords.com)
//

#include <include/popover_menu_button.hpp>
#include <include/log.hpp>

#include <iostream>

namespace mousetrap
{
    PopoverMenu::PopoverMenu(const MenuModel& model)
        : WidgetImplementation<GtkPopoverMenu>(GTK_POPOVER_MENU(
            gtk_popover_menu_new_from_model(model.operator GMenuModel*()))
          ),
          CTOR_SIGNAL(PopoverMenu, closed)
    {
        _model = &model;
        refresh_widgets();
    }

    void PopoverMenu::refresh_widgets()
    {
        for (auto& pair : _model->get_widgets())
        {
            if (gtk_popover_menu_add_child(get_native(), pair.second, pair.first.c_str()) != TRUE)
            {
                // TODO this is triggered but widget insertion seem to be working fine
                continue;

                std::stringstream str;
                str << "In PopoverMenu::refresh_widgets: Failed to add Widget of type " << G_STRINGIFY(pair.second) << " to submenu." << std::endl;
                log::critical(str.str(), MOUSETRAP_DOMAIN);
            }
        }
    }

    PopoverMenuButton::PopoverMenuButton()
        : WidgetImplementation<GtkMenuButton>(GTK_MENU_BUTTON(gtk_menu_button_new())),
          CTOR_SIGNAL(PopoverMenuButton, activate)
    {
        gtk_menu_button_set_always_show_arrow(get_native(), true);
    }

    void PopoverMenuButton::set_child(Widget* child)
    {
        WARN_IF_SELF_INSERTION(PopoverMenuButton::set_child, this, child);

        _child = child;
        gtk_menu_button_set_child(get_native(), _child == nullptr ? nullptr : _child->operator GtkWidget*());

        if (_popover_menu != nullptr)
            _popover_menu->refresh_widgets();
    }

    void PopoverMenuButton::set_popover_position(RelativePosition type)
    {
        gtk_popover_set_position(gtk_menu_button_get_popover(get_native()), (GtkPositionType) type);
    }

    RelativePosition PopoverMenuButton::get_popover_position() const
    {
        return (RelativePosition) gtk_popover_get_position(gtk_menu_button_get_popover(get_native()));
    }

    void PopoverMenuButton::set_popover(Popover& popover)
    {
        _popover = &popover;
        _popover_menu = nullptr;
        gtk_menu_button_set_popover(get_native(), popover.operator GtkWidget*());
    }

    void PopoverMenuButton::set_popover_menu(PopoverMenu& popover_menu)
    {
        _popover = nullptr;
        _popover_menu = &popover_menu;

        gtk_menu_button_set_popover(get_native(), popover_menu.operator GtkWidget*());
        _popover_menu->refresh_widgets();
    }

    void PopoverMenuButton::remove_popover()
    {
        _popover_menu = nullptr;
        gtk_menu_button_set_popover(get_native(), nullptr);
    }

    void PopoverMenuButton::popup()
    {
        gtk_menu_button_popup(get_native());
    }

    void PopoverMenuButton::popdown()
    {
        gtk_menu_button_popdown(get_native());
    }

    void PopoverMenuButton::set_always_show_arrow(bool b)
    {
        gtk_menu_button_set_always_show_arrow(get_native(), b);
    }

    void PopoverMenuButton::set_has_frame(bool b)
    {
        gtk_menu_button_set_has_frame(get_native(), b);
    }
}