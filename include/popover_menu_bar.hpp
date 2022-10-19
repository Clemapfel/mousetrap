// 
// Copyright 2022 Clemens Cords
// Created on 10/19/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/menu_model.hpp>

namespace mousetrap
{
    class PopoverMenuBar : public WidgetImplementation<GtkPopoverMenuBar>
    {
        public:
            PopoverMenuBar(MenuModel*);
    };
}

//

namespace mousetrap
{
    PopoverMenuBar::PopoverMenuBar(MenuModel* model)
        : WidgetImplementation<GtkPopoverMenuBar>(GTK_POPOVER_MENU_BAR(gtk_popover_menu_bar_new_from_model(model->operator GMenuModel*())))
    {
        for (auto& pair : model->get_widgets())
            gtk_popover_menu_bar_add_child(get_native(), pair.second->operator GtkWidget*(), pair.first.c_str());
    }
}