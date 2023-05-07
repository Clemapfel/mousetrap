// 
// Copyright 2022 Clemens Cords
// Created on 10/22/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/event_controller.hpp>
#include <include/action_map.hpp>

#include <iostream>

namespace mousetrap
{
    enum ShortcutScope
    {
        LOCAL = GTK_SHORTCUT_SCOPE_LOCAL, // handled inside the widget the controller belongs to
        MANAGED = GTK_SHORTCUT_SCOPE_MANAGED, // handled by the first ancestor that is a PopoverMenu or Window
        GLOBAL = GTK_SHORTCUT_SCOPE_GLOBAL // always active
    };

    class ShortcutController : public EventController
    {
        public:
            ShortcutController(ActionMap*);
            ~ShortcutController();

            void add_action(ActionID id);
            void remove_action(ActionID id);

            void set_scope(ShortcutScope);
            ShortcutScope get_scope();

        private:
            ActionMap* _action_map;
            std::unordered_map<ActionID, std::vector<GtkShortcut*>> _shortcuts;
    };
}

