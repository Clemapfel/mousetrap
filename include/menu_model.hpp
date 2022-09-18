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
            ~MenuModel();

            void add_action(const std::string& label, const std::string& action_id);
            void add_section(const std::string& label, MenuModel*);
            void add_submenu(const std::string& label, MenuModel*);

            operator GMenuModel*();

        private:
            GMenu* _native;
    };
}

#include <src/menu_model.inl>
