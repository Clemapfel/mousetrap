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

    Action::~Action()
    {}

    void Action::on_action_activate(GSimpleAction*, GVariant*, Action* instance)
    {
        instance->_stateless_f();
    }

    void Action::on_action_change_state(GSimpleAction*, GVariant* variant, Action* instance)
    {
        if (instance->_stateful_bool_f)
        {
            instance->_stateful_bool_f(variant);
            return;
        }
    }

    void Action::initialize_as_stateless()
    {
        _stateful_bool_f = nullptr;

        _g_action = g_object_ref(g_simple_action_new(_id.c_str(), nullptr));
        g_signal_connect(G_OBJECT(_g_action), "activate", G_CALLBACK(on_action_activate), this);
    }

    void Action::initialize_as_stateful_bool(bool initial)
    {
        _stateless_f = nullptr;

        _g_action = g_object_ref(g_simple_action_new_stateful(_id.c_str(), nullptr, g_variant_new_boolean(initial)));
        g_signal_connect(G_OBJECT(_g_action), "change-state", G_CALLBACK(on_action_change_state), this);
    }

    void Action::activate() const
    {
        if (_stateless_f)
        {
            _stateless_f();
            return;
        }

        if (_stateful_bool_f)
        {
            auto* state = g_action_get_state(G_ACTION(_g_action));
            _stateful_bool_f(state);
            g_variant_unref(state);
            return;
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