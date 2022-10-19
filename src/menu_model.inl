// 
// Copyright 2022 Clemens Cords
// Created on 9/18/22 by clem (mail@clemens-cords.com)
//

#include <include/get_resource_path.hpp>

namespace mousetrap
{
    MenuModel::MenuModel()
    {
        _native = g_object_ref(g_menu_new());
    }

    MenuModel::~MenuModel()
    {
        g_object_unref(_native);
    }

    void MenuModel::add_action(const std::string& label, const std::string& action_id)
    {
        auto* item = g_menu_item_new(label.c_str(), action_id.c_str());
        g_menu_append_item(_native, item);
        g_object_unref(item);
    }

    void MenuModel::add_widget(Widget* widget)
    {
        auto id = std::to_string(current_id).c_str();
        auto* item = g_menu_item_new(id, id);
        g_menu_append_item(_native, item);

        _id_to_widget.insert({std::string(id), widget});
        id += 1;
    }

    void MenuModel::add_section(const std::string& label, MenuModel* model)
    {
        g_menu_append_section(_native, label.c_str(), G_MENU_MODEL(model->_native));
    }

    void MenuModel::add_submenu(const std::string& label, MenuModel* model)
    {
        if (model->_submenus.find(this) != model->_submenus.end())
            std::cerr << "[ERROR] In MenuModel::add_submenu: Trying to add menu " << model << " to " << this << ", even though " << this << " is already a submenu of " << model << ". This will create an infinite loop on initialization." << std::endl;

        _submenus.insert(model);
        g_menu_append_submenu(_native, label.c_str(), G_MENU_MODEL(model->_native));
    }

    MenuModel::operator GMenuModel*()
    {
        return G_MENU_MODEL(_native);
    }

    std::unordered_map<std::string, Widget*> MenuModel::get_widgets()
    {
        auto out = std::unordered_map<std::string, Widget*>(_id_to_widget);

        for (auto* submenu : _submenus)
            for (auto& pair : submenu->get_widgets())
                out.insert(pair);

        return out;
    }
}