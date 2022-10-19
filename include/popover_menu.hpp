// 
// Copyright 2022 Clemens Cords
// Created on 10/19/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/menu_model.hpp>

namespace mousetrap
{
    class PopoverMenu : public WidgetImplementation<GtkPopoverMenu>
    {
        public:
            PopoverMenu(MenuModel*, bool use_sliding_layout = true);
    };
}

//

namespace mousetrap
{
    PopoverMenu::PopoverMenu(MenuModel* model, bool use_sliding_layout)
        : WidgetImplementation<GtkPopoverMenu>(GTK_POPOVER_MENU(use_sliding_layout ?
            gtk_popover_menu_new_from_model(model->operator GMenuModel*()) :
            gtk_popover_menu_new_from_model_full(model->operator GMenuModel*(), GTK_POPOVER_MENU_NESTED)
        ))
    {
        for (auto& pair : model->get_widgets())
            if (not gtk_popover_menu_add_child(get_native(), pair.second->operator GtkWidget*(), pair.first.c_str()))
                std::cerr << "[WARNING] In PopoverMenu::PopoverMenu: Failed to add Widget " << pair.second->operator GtkWidget*() << " to submenu." << std::endl;
    }
}