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

            void create_from_group(const std::string& group, ConfigFile* file);
            size_t get_n_shortcuts();

        private:
            std::string _title;
            ShortcutGroupDisplay* _shortcuts = nullptr;

            std::vector<std::pair<std::string, std::string>> _accelerators_and_descriptions;

            Box _main = Box(GTK_ORIENTATION_VERTICAL);
    };
}

///

namespace mousetrap
{
    ShortcutInformation::ShortcutInformation()
    {}

    void ShortcutInformation::update()
    {
        _main.clear();

        delete _shortcuts;
        _shortcuts = new ShortcutGroupDisplay(_title, _accelerators_and_descriptions);

        _main.clear();
        _main.push_back(_shortcuts);
    }

    ShortcutInformation::operator Widget*()
    {
        return &_main;
    }

    ShortcutInformation::~ShortcutInformation()
    {
        delete _shortcuts;
    }

    void ShortcutInformation::set_title(const std::string& text)
    {
        _title = text;
    }

    void ShortcutInformation::add_shortcut(const std::string& accelerator, const std::string& description)
    {
        _accelerators_and_descriptions.emplace_back(accelerator, description);
        update();
    }

    void ShortcutInformation::create_from_group(const std::string& group, ConfigFile* file)
    {
        auto keys = file->get_keys(group);
        _accelerators_and_descriptions.clear();

        for (auto& key : keys)
            _accelerators_and_descriptions.emplace_back(file->get_value(group, key), file->get_comment_above(group, key));

        update();
    }

    size_t ShortcutInformation::get_n_shortcuts()
    {
        return _accelerators_and_descriptions.size();
    }
}
