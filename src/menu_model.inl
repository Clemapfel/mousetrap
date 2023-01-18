// 
// Copyright 2022 Clemens Cords
// Created on 9/18/22 by clem (mail@clemens-cords.com)
//

#include <include/get_resource_path.hpp>

namespace mousetrap
{
    inline MenuModel::MenuModel()
    {
        _native = g_object_ref(g_menu_new());
    }

    inline MenuModel::~MenuModel()
    {
        g_object_unref(_native);
    }

    inline void MenuModel::add_action(const std::string& label, const std::string& action_id, bool use_markup)
    {
        auto* item = g_menu_item_new(label.c_str(), ("app." + action_id).c_str());
        g_menu_item_set_attribute_value(item, "use-markup", g_variant_new_string(use_markup ? "yes" : "no"));
        g_menu_append_item(_native, item);
        g_object_unref(item);
    }

    inline void MenuModel::add_widget(Widget* widget)
    {
        auto id = std::to_string(current_id).c_str();
        auto* item = g_menu_item_new(id, id);
        g_menu_item_set_attribute_value(item, "custom", g_variant_new_string(id));

        g_menu_append_item(_native, item);

        _id_to_widget.insert({std::string(id), widget});
        current_id += 1;
    }

    inline void MenuModel::add_section(const std::string& label, MenuModel* model, MenuSectionFormat format)
    {
        auto* item = g_menu_item_new_section(label.c_str(), G_MENU_MODEL(model->_native));

        if (format == HORIZONTAL_BUTTONS)
        {
            g_menu_item_set_attribute_value(item, "display-hint", g_variant_new_string("horizontal-buttons"));
        }
        else if (format == HORIZONTAL_BUTTONS_LEFT_TO_RIGHT)
        {
            g_menu_item_set_attribute_value(item, "display-hint", g_variant_new_string("horizontal-buttons"));
            g_menu_item_set_attribute_value(item, "text-direction", g_variant_new_string("ltr"));
        }
        else if (format == HORIZONTAL_BUTTONS_RIGHT_TO_LEFT)
        {
            g_menu_item_set_attribute_value(item, "display-hint", g_variant_new_string("horizontal-buttons"));
            g_menu_item_set_attribute_value(item, "text-direction", g_variant_new_string("rtl"));
        }
        else if (format == CIRCULAR_BUTTONS)
        {
            g_menu_item_set_attribute_value(item, "display-hint", g_variant_new_string("circular-buttons"));
        }
        else if (format == INLINE_BUTTONS)
        {
            g_menu_item_set_attribute_value(item, "display-hint", g_variant_new_string("inline-buttons"));
        }
        else
        {
            // noop
        }

        _submenus.insert(model);

        g_menu_append_item(_native, item);
    }

    inline void MenuModel::add_submenu(const std::string& label, MenuModel* model)
    {
        if (model->_submenus.find(this) != model->_submenus.end())
            std::cerr << "[ERROR] In MenuModel::add_submenu: Trying to add menu " << model << " to " << this << ", even though " << this << " is already a submenu of " << model << ". This will create an infinite loop on initialization." << std::endl;

        _submenus.insert(model);

        auto* item = g_menu_item_new_submenu(label.c_str(), G_MENU_MODEL(model->_native));
        g_menu_append_item(_native, item);
    }

    inline MenuModel::operator GMenuModel*()
    {
        return G_MENU_MODEL(_native);
    }

    inline std::unordered_map<std::string, Widget*> MenuModel::get_widgets()
    {
        auto out = std::unordered_map<std::string, Widget*>(_id_to_widget);

        for (auto* submenu : _submenus)
            for (auto& pair : submenu->get_widgets())
                out.insert(pair);

        return out;
    }
}