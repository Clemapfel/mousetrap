//
// Copyright 2022 Clemens Cords
// Created on 10/22/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/event_controller.hpp>
#include <include/application.hpp>

#include <unordered_map>

namespace mousetrap
{
    /// @brief declares scope in which a shortcut controller should capture shortcut events
    enum ShortcutScope
    {
        /// @brief captured by widget the controller belongs to
        LOCAL = GTK_SHORTCUT_SCOPE_LOCAL,

        /// @brief captured by first ancestor that is a PopoverMenu or Window
        MANAGED = GTK_SHORTCUT_SCOPE_MANAGED,

        /// @brief always captured
        GLOBAL = GTK_SHORTCUT_SCOPE_GLOBAL
    };

    /// @brief event controller that manages shortcuts, you do not need to connect to any signals of this class
    class ShortcutController : public EventController
    {
        public:
            /// @brief construct
            /// @param app Application actions are registered to
            ShortcutController();

            /// @brief add action, if the action has a shortcut and it is registered with the application, shortcut controller will activate the action when the shortcut is pressed
            /// @param action
            void add_action(const Action& action);

            /// @brief set the controllers scope, ShortcutScope::GLOBAL by default
            /// @param scope
            void set_scope(ShortcutScope);

            /// @brief get the controllers scope
            /// @return scope
            ShortcutScope get_scope();
    };
}

