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
            void update();

            void set_title(const std::string&);
            void add_shortcut(const std::string& accelerator, const std::string& description);

            void create_from_group(const std::string& section, ConfigFile* file);

        private:
           std::string _title;
           std::vector<std::pair<std::string, std::string>> _accelerators_and_description;

           ShortcutGroupDisplay* _display = nullptr;
           Box _main = Box(GTK_ORIENTATION_HORIZONTAL);
    };
}

///

namespace mousetrap
{
    ShortcutInformation::ShortcutInformation()
    {}

    ShortcutInformation::operator Widget*()
    {
        return &_main;
    }

    ShortcutInformation::~ShortcutInformation()
    {
        delete _display;
    }

    void ShortcutInformation::update()
    {
        _main.clear();
        delete _display;

        _display = new ShortcutGroupDisplay(_title, _accelerators_and_description);
        _main.push_back(_display);
    }

    void ShortcutInformation::set_title(const std::string& title)
    {
        _title = title;
        update();
    }

    void ShortcutInformation::add_shortcut(const std::string& accelerator, const std::string& description)
    {
        _accelerators_and_description.emplace_back(accelerator, description);
        update();
    }

    void ShortcutInformation::create_from_group(const std::string& section, ConfigFile* file)
    {
        auto keys = file->get_keys(section);
        _accelerators_and_description.clear();

        for (auto key : keys)
            _accelerators_and_description.emplace_back(file->get_value(section, key), file->get_comment_above(section, key));

        update();
    }
}
