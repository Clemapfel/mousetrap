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
            ~Action() = default;

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

        protected:
            virtual operator GtkApplication*() = 0;

        private:
            std::unordered_map<ActionID, Action> _actions;
    };
}

///
namespace mousetrap
{
    Action::Action(const std::string& id)
        : _id(id)
    {}

    template<typename DoFunction_t, typename DoData_t>
    void Action::set_do_function(DoFunction_t f_in, DoData_t data_in)
    {
        _do = [f = f_in, data = data_in]() {
            f(data);
        };

        _g_action = g_simple_action_new(_id.c_str(), nullptr);
        g_signal_connect(G_OBJECT(_g_action), "activate", G_CALLBACK(on_action_activate), this);
    }

    template<typename DoFunction_t>
    void Action::set_do_function(DoFunction_t f_in)
    {
        _do = [f = f_in]() {
            f();
        };

        _g_action = g_simple_action_new(_id.c_str(), nullptr);
        g_signal_connect(G_OBJECT(_g_action), "activate", G_CALLBACK(on_action_activate), this);
    }

    template<typename UndoFunction_t, typename UndoData_t>
    void Action::set_undo_function(UndoFunction_t f_in, UndoData_t data_in)
    {
        _undo = [f = f_in, data = data_in]() {
            f(data);
        };
    }

    template<typename UndoFunction_t>
    void Action::set_undo_function(UndoFunction_t f_in)
    {
        _undo = [f = f_in]() {
            f();
        };
    }

    void Action::on_action_activate(GSimpleAction*, GVariant*, Action* instance)
    {
        instance->activate();
    }

    void Action::activate() const
    {
        if (_do != nullptr)
            _do();
    }

    void Action::undo() const
    {
        if (_undo != nullptr)
            _undo();
    }

    void Action::add_shortcut(const ShortcutTriggerID& shortcut)
    {
        if (gtk_shortcut_trigger_parse_string(shortcut.c_str()) == nullptr)
        {
            std::cerr << "[WARNING] In Action::add_shortcut: Unable to parse shortcut trigger `" << shortcut
                      << "`. Ignoring this shortcut binding" << std::endl;
            return;
        }

        _shortcuts.push_back(shortcut.c_str());
    }

    const std::vector<ShortcutTriggerID>& Action::get_shortcuts() const
    {
        return _shortcuts;
    }

    Action::operator GAction*() const
    {
        return G_ACTION(_g_action);
    }

    ActionID Action::get_id() const
    {
        return _id;
    }

    void ActionMap::add_action(Action& action)
    {
        auto inserted = _actions.insert({action.get_id(), action}).first->second;

        auto* self = operator GActionMap*();
        g_action_map_add_action(self, inserted.operator GAction *());

        auto* app = operator GtkApplication*();
        auto accels = std::vector<const char*>();
        for (auto& s : inserted.get_shortcuts())
            accels.push_back(s.c_str());

        gtk_application_set_accels_for_action(app, ("app." + inserted.get_id()).c_str(), accels.data());
    }

    void ActionMap::remove_action(const ActionID& id)
    {
        auto* self = operator GActionMap*();
        _actions.erase(id);
        g_action_map_remove_action(self, ("app." + id).c_str());
    }

    const Action& ActionMap::get_action(const ActionID& id)
    {
        return _actions.at(id);
    }
}