// 
// Copyright 2022 Clemens Cords
// Created on 11/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

#include <app/shortcut_information.hpp>

namespace mousetrap
{
    class Tooltip
    {
        public:
            Tooltip();
            operator Widget*();

            /// \brief reads tooltips.ini for `title` and `descriptions`, as well as keybindings.ini
            void create_from(const std::string& group_name, KeyFile* tooltip_file, KeyFile* keybindings_file);

            void set_title(const std::string&);
            void set_description(const std::string&);
            void create_shortcut_information_from(const std::string& group, KeyFile* file);
            void add_shortcut(const std::string& shortcut, const std::string& description);

        private:
            Revealer _window;
            Box _box = Box(GTK_ORIENTATION_VERTICAL);

            void reformat();

            std::string _title;
            std::string _description;

            Label _title_label;
            Label _description_label;

            ShortcutInformation _shortcut_information;
    };
}
