//
// Copyright 2022 Clemens Cords
// Created on 11/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>
#include <app/app_component.hpp>

namespace mousetrap
{
    class ShortcutInformation : public AppComponent
    {
        public:
            ShortcutInformation();
            ~ShortcutInformation();

            operator Widget*();
            void update() override;

            void set_title(const std::string&);
            void add_shortcut(const std::string& accelerator, const std::string& description);

            void create_from_group(const std::string& group, KeyFile* file);
            size_t get_n_shortcuts();

        private:
            std::string _title;
            ShortcutGroupDisplay* _shortcuts = nullptr;

            std::vector<std::pair<std::string, std::string>> _accelerators_and_descriptions;

            Box _main = Box(GTK_ORIENTATION_VERTICAL);
    };
}
