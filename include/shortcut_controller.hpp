// 
// Copyright 2022 Clemens Cords
// Created on 10/22/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/event_controller.hpp>
#include <include/action_map.hpp>

namespace mousetrap
{
    using ShortcutCode = std::string;

    class ShortcutController : public EventController
    {
        public:
            ShortcutController();
            void add_shortcut(const std::string& shortcut_code, Action& action);
    };
}

//

namespace mousetrap
{
    ShortcutController::ShortcutController()
        : EventController(gtk_shortcut_controller_new())
    {}

    void ShortcutController::add_shortcut(const std::string& shortcut_code, Action& action)
    {

    }
}