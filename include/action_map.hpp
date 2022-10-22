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
    using ActionID = std::string;

    class Action
    {
        public:
            Action(const std::string& id);
            ~Action() = default;

            ActionID get_id();

            template<typename DoFunction_t, typename DoData_t>
            void set_do_function(DoFunction_t, DoData_t);

            template<typename DoFunction_t>
            void set_do_function(DoFunction_t);

            template<typename UndoFunction_t, typename UndoData_t>
            void set_undo_function(UndoFunction_t, UndoData_t);

            template<typename UndoFunction_t>
            void set_undo_function(UndoFunction_t);

            void activate();
            void undo();

            operator GAction*();

        private:
            ActionID _id;

            std::function<void()> _do;
            std::function<void()> _undo;

            static void on_action_activate(GSimpleAction*, GVariant*, Action* instance);
            GSimpleAction* _g_action = nullptr;
    };

    class ActionMap
    {
        public:
            virtual operator GActionMap*() = 0;

            void add_action(Action action);
            void remove_action(const ActionID& id);
            Action& get_action(const ActionID& id);

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

    void Action::activate()
    {
        if (_do != nullptr)
            _do();
    }

    void Action::undo()
    {
        if (_undo != nullptr)
            _undo();
    }

    Action::operator GAction*()
    {
        return G_ACTION(_g_action);
    }

    ActionID Action::get_id()
    {
        return _id;
    }

    void ActionMap::add_action(Action action)
    {
        auto inserted = _actions.insert({action.get_id(), action});

        auto* self = operator GActionMap*();
        g_action_map_add_action(self, inserted.first->second.operator GAction *());
    }

    void ActionMap::remove_action(const ActionID& id)
    {
        auto* self = operator GActionMap*();
        _actions.erase(id);
        g_action_map_remove_action(self, ("app." + id).c_str());
    }

    Action& ActionMap::get_action(const ActionID& id)
    {
        return _actions.at(id);
    }
}