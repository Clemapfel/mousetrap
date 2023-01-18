// 
// Copyright 2022 Clemens Cords
// Created on 10/26/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    inline PopoverMenuBar::PopoverMenuBar(MenuModel* model)
            : WidgetImplementation<GtkPopoverMenuBar>(GTK_POPOVER_MENU_BAR(gtk_popover_menu_bar_new_from_model(model->operator GMenuModel*())))
    {
        for (auto& pair : model->get_widgets())
            if (not gtk_popover_menu_bar_add_child(get_native(), pair.second->operator GtkWidget*(), pair.first.c_str()))
                std::cerr << "[WARNING] In PopoverMenuBar::PopoverMenuBar: Failed to add Widget " << pair.second->operator GtkWidget*() << " to menubar submenu." << std::endl;
    }
}