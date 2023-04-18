//
// Created by clem on 4/10/23.
//

#include <include/menubar.hpp>
#include <include/log.hpp>

namespace mousetrap
{
    MenuBar::MenuBar(const MenuModel& model)
    : WidgetImplementation<GtkPopoverMenuBar>(GTK_POPOVER_MENU_BAR(gtk_popover_menu_bar_new_from_model(model.operator GMenuModel*()))
    )
    {
        _model = &model;
        refresh_widgets();
    }

    void MenuBar::refresh_widgets()
    {
        if (_model->_internal->has_widget_in_toplevel)
            log::warning("In MenuBar::refresh_widgets: MenuModel contains a widget added with MenuModel::add_widget. These cannot be displayed, instead add the widget to a new menu model and add it as a submenu to the MenuBars menu model.", MOUSETRAP_DOMAIN);
            // TODO clarify this

        for (auto& pair : _model->get_widgets())
        {
            if (gtk_popover_menu_bar_add_child(get_native(), pair.second, pair.first.c_str()) != TRUE)
            {
                continue;

                std::stringstream str;
                str << "In PopoverMenuBar::refresh_widgets: Failed to add Widget of type " << G_STRINGIFY(pair.second) << " to submenu." << std::endl;
                log::critical(str.str(), MOUSETRAP_DOMAIN);
            }
        }
    }
}
