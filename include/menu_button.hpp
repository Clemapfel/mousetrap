// 
// Copyright 2022 Clemens Cords
// Created on 8/29/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class MenuButton : public Widget
    {
        public:
            MenuButton();

            operator GtkWidget*();
            void set_child(Widget*);

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
    }

    MenuButton::operator GtkWidget*()
    {
        return GTK_WIDGET(_native);
    }

    void MenuButton::set_child(Widget* widget)
    {
        gtk_menu_button_set_child(_native, widget->operator GtkWidget*());
    }
}