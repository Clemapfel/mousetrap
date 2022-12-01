// 
// Copyright 2022 Clemens Cords
// Created on 10/22/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

#include <string>
#include <functional>

namespace mousetrap
{
    // SHORTCUTS
    // Add a shortcut via Action::get_shortcut. Then, any menu item that uses that same action will automatically
    // display first shortcut for that action.
    // For the shortcut to be activatable by the user, a widget has to add a ShortcutController to it's event
    // controllers. Then, it will react to the user pressing the correct shortcut by triggering the action.

    using ActionID = std::string;
    using ShortcutTriggerID = std::string;

    class Action
    {
        public:
            Action(const std::string& id);
            ~Action();

            ActionID get_id() const;

            template<typename DoFunction_t, typename DoData_t>
            void set_do_function(DoFunction_t, DoData_t);

            template<typename DoFunction_t>
            void set_do_function(DoFunction_t);

            template<typename UndoFunction_t, typename UndoData_t>
            void set_undo_function(UndoFunction_t, UndoData_t);

            template<typename UndoFunction_t>
            void set_undo_function(UndoFunction_t);

            void activate() const;
            void undo() const;

            void add_shortcut(const ShortcutTriggerID&);
            const std::vector<ShortcutTriggerID>& get_shortcuts() const;

            operator GAction*() const;

        private:
            ActionID _id;

            std::function<void()> _do;
            std::function<void()> _undo;

            std::vector<ShortcutTriggerID> _shortcuts;

            static void on_action_activate(GSimpleAction*, GVariant*, Action* instance);
            GSimpleAction* _g_action = nullptr;
    };

    class ActionMap
    {
        public:
            virtual operator GActionMap*() = 0;

            void add_action(Action& action);
            void remove_action(const ActionID& id);
            const Action& get_action(const ActionID& id);
            bool has_action(const ActionID& id);

        protected:
            virtual operator GtkApplication*() = 0;

        private:
            std::unordered_map<ActionID, Action> _actions;
    };
}

#include <src/action_map.inl>
