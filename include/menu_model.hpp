// 
// Copyright 2022 Clemens Cords
// Created on 8/29/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

#include <string>
#include <set>

namespace mousetrap
{
    class MenuModel
    {
        public:
            MenuModel();
            ~MenuModel();

            void add_action(
                const std::string& label,
                const std::string& action_id,
                bool use_markup = true);

            void add_widget(Widget*);

            enum MenuSectionFormat
            {
                NORMAL,
                HORIZONTAL_BUTTONS,
                HORIZONTAL_BUTTONS_LEFT_TO_RIGHT,
                HORIZONTAL_BUTTONS_RIGHT_TO_LEFT,
                CIRCULAR_BUTTONS,
                INLINE_BUTTONS
            };

            void add_section(
                const std::string& label,
                MenuModel*,
                MenuSectionFormat = NORMAL);

            void add_submenu(const std::string& label, MenuModel*);

            operator GMenuModel*();
            std::unordered_map<std::string, Widget*> get_widgets();

        private:
            GMenu* _native;

            static inline size_t current_id = 1;
            std::unordered_map<std::string, Widget*> _id_to_widget;
            std::set<MenuModel*> _submenus;
    };
}

#include <src/menu_model.inl>
