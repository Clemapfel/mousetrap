//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/13/23
//

#pragma once

#include <gtk/gtk.h>
#include <string>
#include <functional>
#include <deque>

namespace mousetrap
{
    // SHORTCUTS
    // Add a shortcut via Action::get_shortcut. Then, any menu item that uses that same action will automatically
    // display first shortcut for that action.
    // For the shortcut to be activatable by the user, a widget has to add a ShortcutController to it's event
    // controllers. Then, it will react to the user pressing the correct shortcut by triggering the action.

    using ActionID = std::string;
    using ShortcutTriggerID = std::string;

    #define DECLARE_GLOBAL_ACTION(group, key) inline mousetrap::Action group##_##key = Action(#group + std::string(".") + #key);

    class Action
    {
        public:
            Action(const std::string& id);
            Action(const std::string& id, bool* state);
            ~Action();

            ActionID get_id() const;

            template<typename DoFunction_t>
            void set_function(DoFunction_t do_function);

            template<typename DoFunction_t, typename UndoFunction_t, typename RedoFunction_t>
            void set_function(DoFunction_t do_function, UndoFunction_t undo_function, RedoFunction_t redo_function);

            void activate() const;
            void undo() const;
            void redo() const;

            void add_shortcut(const ShortcutTriggerID&);
            const std::vector<ShortcutTriggerID>& get_shortcuts() const;

            operator GAction*() const;
            void set_enabled(bool);
            bool get_enabled() const;

        private:
            ActionID _id;

            struct InstanceState
            {
                InstanceState() = default;

                std::deque<std::function<void()>>* undo_queue = new std::deque<std::function<void()>>{};
                std::deque<std::function<void()>>* redo_queue = new std::deque<std::function<void()>>{};
                std::function<void()>* do_f = new std::function<void()>([](){});
                std::function<void()>* undo_f = new std::function<void()>([](){});
                std::function<void()>* redo_f = new std::function<void()>([](){});
            };
            static inline std::unordered_map<ActionID, InstanceState*> _instance_states = {};

            std::vector<ShortcutTriggerID> _shortcuts;

            static void on_action_activate(GSimpleAction*, GVariant*, Action*);
            GSimpleAction* _g_action = nullptr;
            bool _enabled = true;

            static void on_action_change_state(GSimpleAction*, GVariant*, Action*);
    };
}

namespace mousetrap
{
    template<typename DoFunction_t>
    void Action::set_function(DoFunction_t do_function)
    {
        _instance_states.insert({_id, new InstanceState()});
        auto* state = _instance_states.at(_id);

        (*state->do_f) = [f = std::function<void()>(do_function)](){
            f();
        };

        (*state->undo_f) = [](){};
        (*state->redo_f) = [](){};

        _g_action = g_object_ref(g_simple_action_new(_id.c_str(), nullptr));
        g_signal_connect(G_OBJECT(_g_action), "activate", G_CALLBACK(on_action_activate), this);
        set_enabled(_enabled);
    }

    template<typename DoFunction_t, typename UndoFunction_t, typename RedoFunction_t>
    void Action::set_function(DoFunction_t do_function, UndoFunction_t undo_function, RedoFunction_t redo_function)
    {
        _instance_states.insert({_id, new InstanceState()});
        auto* state = _instance_states.at(_id);

        (*state->do_f) = [f = std::function<void()>(do_function), state](){
            f();
            state->undo_queue->emplace_back([&](){
                (*state->undo_f)();
            });
        };

        (*state->undo_f) = [f = std::function<void()>(undo_function), state](){
            f();
            state->redo_queue->emplace_back([&](){
                (*state->redo_f)();
            });
        };

        (*state->redo_f) = [f = std::function<void()>(redo_function), state](){
            f();
            state->undo_queue->emplace_back([&](){
                (*state->undo_f)();
            });
        };

        _g_action = g_object_ref(g_simple_action_new(_id.c_str(), nullptr));
        g_signal_connect(G_OBJECT(_g_action), "activate", G_CALLBACK(on_action_activate), this);
        set_enabled(_enabled);
    }
}
