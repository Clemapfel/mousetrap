// 
// Copyright 2022 Clemens Cords
// Created on 9/18/22 by clem (mail@clemens-cords.com)
//

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

    void MenuModel::add_section(const std::string& label, MenuModel* model)
    {
        g_menu_append_section(_native, label.c_str(), G_MENU_MODEL(model->_native));
    }

    void MenuModel::add_submenu(const std::string& label, MenuModel* model)
    {
        g_menu_append_submenu(_native, label.c_str(), G_MENU_MODEL(model->_native));
    }

    MenuModel::operator GMenuModel*()
    {
        return G_MENU_MODEL(_native);
    }
}