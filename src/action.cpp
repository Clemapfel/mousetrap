//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/13/23
//

#include <include/action.hpp>

#include <iostream>

namespace mousetrap
{
    Action::Action(const std::string& id)
        : _id(id)
    {}

    Action::Action(const std::string& id, bool* state)
        : _id(id)
    {
        auto* variant = g_variant_new_boolean(true);
        _g_action = g_object_ref(g_simple_action_new_stateful(_id.c_str(), NULL, variant));
        g_signal_connect(G_OBJECT(_g_action), "change-state", G_CALLBACK(on_action_change_state), this);
        set_enabled(_enabled);
    }

    Action::~Action()
    {}

    void Action::on_action_activate(GSimpleAction*, GVariant*, Action* instance)
    {
        instance->activate();
    }

    void Action::on_action_change_state(GSimpleAction*, GVariant* variant, Action*)
    {
        std::cout << g_variant_get_boolean(variant) << std::endl;
    }

    void Action::activate() const
    {
        auto& state = _instance_states.at(_id);
        if (state->do_f)
            (*state->do_f)();
    }

    void Action::undo() const
    {
        auto& state = _instance_states.at(_id);
        if (not state->undo_queue->empty())
        {
            state->undo_queue->back()();
            state->undo_queue->pop_back();
        }
    }

    void Action::redo() const
    {
        auto& state = _instance_states.at(_id);
        if (not state->redo_queue->empty())
        {
            state->redo_queue->back()();
            state->redo_queue->pop_back();
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

    void Action::set_enabled(bool b)
    {
        _enabled = b;

        if (_g_action != nullptr)
            g_simple_action_set_enabled(_g_action, b);
    }

    bool Action::get_enabled() const
    {
        return _enabled;
    }
}