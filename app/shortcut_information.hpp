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
            void update() override {};

            void set_title(const std::string&);
            void set_description(const std::string&);
            void add_shortcut(const std::string& accelerator, const std::string& description);

        private:
            Label _title = Label("");
            Label _description = Label("");
            std::vector<ShortcutView*> _shortcut_views;

            Box _main = Box(GTK_ORIENTATION_HORIZONTAL);
    };
}

///

namespace mousetrap
{
    ShortcutInformation::ShortcutInformation()
    {
        _main.push_back(&_title);
        _main.push_back(&_description);
    }

    ShortcutInformation::operator Widget*()
    {
        return &_main;
    }

    ShortcutInformation::~ShortcutInformation()
    {
        for (auto* view : _shortcut_views)
            delete view;
    }

    void ShortcutInformation::set_title(const std::string& text)
    {
        _title.set_text("<b>" + text + "</b>");
    }

    void ShortcutInformation::set_description(const std::string& text)
    {
        _description.set_text(text);
    }

    void ShortcutInformation::add_shortcut(const std::string& accelerator, const std::string& description)
    {
        _shortcut_views.emplace_back(new ShortcutView("  " + accelerator, description));
        _main.push_back(_shortcut_views.back());
    }
}
