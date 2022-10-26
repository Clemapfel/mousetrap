// 
// Copyright 2022 Clemens Cords
// Created on 10/26/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    PopoverMenu::PopoverMenu(MenuModel* model, SubmenuLayout layout)
            : WidgetImplementation<GtkPopoverMenu>(GTK_POPOVER_MENU(layout == SLIDING ?
                                                                    gtk_popover_menu_new_from_model(model->operator GMenuModel*()) :
                                                                    gtk_popover_menu_new_from_model_full(model->operator GMenuModel*(), GTK_POPOVER_MENU_NESTED)
    ))
    {
        for (auto& pair : model->get_widgets())
        {
            if (not gtk_popover_menu_add_child(get_native(), pair.second->operator GtkWidget*(), pair.first.c_str()))
                std::cerr << "[WARNING] In PopoverMenu::PopoverMenu: Failed to add Widget "
                          << pair.second->operator GtkWidget*() << " to submenu." << std::endl;
        }
    }
}