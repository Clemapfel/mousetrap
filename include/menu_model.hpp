// 
// Copyright 2022 Clemens Cords
// Created on 8/29/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

#include <string>

namespace mousetrap
{
    class MenuModel
    {
        public:
            MenuModel();

            void add_action(const std::string& label, const std::string& action_id);
            void add_section(const std::string& label, MenuModel*);
            void add_submenu(const std::string& label, MenuModel*);

            operator GMenuModel*();

        private:
            GMenu* _native;
    };
}

// ###

namespace mousetrap
{
    MenuModel::MenuModel()
    {
        _native = g_menu_new();
    }

    void MenuModel::add_action(const std::string& label, const std::string& action_id)
    {
        auto* item = g_menu_item_new(label.c_str(), action_id.c_str());
        g_menu_append_item(_native, item);
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