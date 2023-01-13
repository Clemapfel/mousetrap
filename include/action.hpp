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

    class Action
    {
        public:
            Action(const std::string& id);
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

        private:
            ActionID _id;

            struct InstanceState
            {
                InstanceState() = default;
                ~InstanceState();

                std::deque<std::function<void()>> undo_queue = {};
                std::deque<std::function<void()>> redo_queue = {};
                std::function<void()>* do_f = nullptr;
                std::function<void()>* undo_f = nullptr;
                std::function<void()>* redo_f = nullptr;
            };
            static std::unordered_map<ActionID, InstanceState*> _instance_states;

            std::vector<ShortcutTriggerID> _shortcuts;

            static void on_action_activate(GSimpleAction*, GVariant*, Action* instance);
            GSimpleAction* _g_action = nullptr;
    };
}

// #include <source/action.inl>

namespace mousetrap
{
    Action::Action(const std::string& id)
            : _id(id)
    {
        _instance_states.insert({_id, new InstanceState()});
    }

    Action::InstanceState::~InstanceState()
    {
        delete do_f;
        delete undo_f;
        delete redo_f;
    }

    Action::~Action()
    {
        g_object_unref(_g_action);
        delete _instance_states.at(_id);
        _instance_states.erase(_id);
    }

    template<typename DoFunction_t>
    void Action::set_function(DoFunction_t do_function)
    {
        auto* state = _instance_states.at(_id);

        (*state->do_f) = [&](){
            do_function();
        };

        (*state->undo_f) = [&](){};
        (*state->redo_f) = [&](){};

        _g_action = g_object_ref(g_simple_action_new(_id.c_str(), nullptr));
        g_signal_connect(G_OBJECT(_g_action), "activate", G_CALLBACK(on_action_activate), this);
    }

    template<typename DoFunction_t, typename UndoFunction_t, typename RedoFunction_t>
    void Action::set_function(DoFunction_t do_function, UndoFunction_t undo_function, RedoFunction_t redo_function)
    {
        auto* state = _instance_states.at(_id);

        (*state->do_f) = [&](){
            do_function();
            state->undo_queue.emplace_back([&](){
                (*state->undo_f)();
            });
        };

        (*state->undo_f) = [&](){
            undo_function();
            state->redo_queue.emplace_back([&](){
                (*state->redo_f)();
            });
        };

        (*state->redo_f) = [&](){
            redo_function();
            state->undo_queue.emplace_back([&](){
                (*state->undo_f)();
            });
        };

        _g_action = g_object_ref(g_simple_action_new(_id.c_str(), nullptr));
        g_signal_connect(G_OBJECT(_g_action), "activate", G_CALLBACK(on_action_activate), this);
    }

    void Action::on_action_activate(GSimpleAction*, GVariant*, Action* instance)
    {
        instance->activate();
    }

    void Action::activate() const
    {
        auto* state = _instance_states.at(_id);
        if (state->do_f)
            (*state->do_f)();
    }

    void Action::undo() const
    {
        auto* state = _instance_states.at(_id);
        if (not state->undo_queue.empty())
        {
            state->undo_queue.back()();
            state->undo_queue.pop_back();
        }
    }

    void Action::redo() const
    {
        auto* state = _instance_states.at(_id);
        if (not state->redo_queue.empty())
        {
            state->redo_queue.back()();
            state->redo_queue.pop_back();
        }
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
}
