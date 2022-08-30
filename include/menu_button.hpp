// 
// Copyright 2022 Clemens Cords
// Created on 8/29/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/menu_model.hpp>

namespace mousetrap
{
    class MenuButton : public Widget
    {
        public:
            MenuButton();

            operator GtkWidget*();
            void set_child(Widget*);
            void set_model(MenuModel*);

            void set_popover_position(GtkPositionType);

        private:
            GtkMenuButton* _native;
    };
}

//#include <src/menu_button.inl>

namespace mousetrap
{
    MenuButton::MenuButton()
    {
        _native = GTK_MENU_BUTTON(gtk_menu_button_new());
        gtk_menu_button_set_always_show_arrow(_native, true);
    }

    MenuButton::operator GtkWidget*()
    {
        return GTK_WIDGET(_native);
    }

    void MenuButton::set_child(Widget* widget)
    {
        gtk_menu_button_set_child(_native, widget->operator GtkWidget*());
    }

    void MenuButton::set_model(MenuModel* model)
    {
        gtk_menu_button_set_menu_model(_native, model->operator GMenuModel*());
    }

    void MenuButton::set_popover_position(GtkPositionType type)
    {
        gtk_popover_set_position(gtk_menu_button_get_popover(_native), type);
    }
}