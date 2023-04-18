//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/13/23
//

#include <include/action.hpp>
#include <include/application.hpp>
#include <include/log.hpp>

#include <iostream>

namespace mousetrap::detail
{
    DECLARE_NEW_TYPE(ActionInternal, action_internal, ACTION_INTERNAL)
    DEFINE_NEW_TYPE_TRIVIAL_FINALIZE(ActionInternal, action_internal, ACTION_INTERNAL)
    DEFINE_NEW_TYPE_TRIVIAL_INIT(ActionInternal, action_internal, ACTION_INTERNAL)
    DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(ActionInternal, action_internal, ACTION_INTERNAL)

    static ActionInternal* action_internal_new(const std::string& in)
    {
        auto* self = (ActionInternal*) g_object_new(action_internal_get_type(), nullptr);
        action_internal_init(self);

        self->id = in;
        self->shortcuts = {};
        self->g_action = nullptr;
        self->g_state = nullptr;
        self->stateless_f = nullptr;
        self->stateful_f = nullptr;
        self->enabled = true;
        return self;
    }
}

namespace mousetrap
{
    Action::Action(const std::string& id)
        : Action(detail::action_internal_new(id))
    {}

    Action::Action(detail::ActionInternal* internal)
        : _internal(internal)
    {
        g_object_ref(_internal);
    }

    void Action::on_action_activate(GSimpleAction*, GVariant* variant, detail::ActionInternal* instance)
    {
        if (instance->stateless_f)
            instance->stateless_f();

        if (instance->stateful_f)
            instance->stateful_f();
    }

    Action::~Action()
    {
        if (G_IS_OBJECT(_internal))
            g_object_unref(_internal);
    }

    void Action::on_action_change_state(GSimpleAction*, GVariant* variant, detail::ActionInternal* instance)
    {
        if (instance->stateless_f)
            instance->stateless_f();

        if (instance->stateful_f)
            instance->stateful_f();
    }

    void Action::activate() const
    {
        if (_internal->stateless_f)
            (_internal->stateless_f)();
        else if (_internal->stateful_f)
            (_internal->stateful_f)();

        if (not _internal->stateful_f and not _internal->stateless_f)
            log::warning("In Action::activate: Activating action with id " + get_id() + ", but set_function or setstateful_function has not been called yet", MOUSETRAP_DOMAIN);
    }

    void Action::set_state(bool b)
    {
        if (get_is_stateful())
            g_action_change_state(G_ACTION(_internal->g_action), g_variant_new_boolean(b));
        else
            log::warning("In Action::set_state: Action with id " + get_id() + " is stateless", MOUSETRAP_DOMAIN);
    }

    bool Action::get_state() const
    {
        if (get_is_stateful())
            return g_variant_get_boolean(g_action_get_state(G_ACTION(_internal->g_action)));
        else
        {
            log::warning("In Action::get_state: Action with id " + get_id() + " is stateless", MOUSETRAP_DOMAIN);
            return false;
        }
    }

    void Action::add_shortcut(const ShortcutTriggerID& shortcut)
    {
        auto* trigger = gtk_shortcut_trigger_parse_string(shortcut.c_str());
        if (trigger == nullptr)
        {
            auto str = std::stringstream();
            str << "In Action::add_shortcut: Unable to parse shortcut trigger `" << shortcut
                      << "`. Ignoring this shortcut binding" << std::endl;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            return;
        }

        g_object_unref(trigger);
        _internal->shortcuts.push_back(shortcut.c_str());
    }

    const std::vector<ShortcutTriggerID>& Action::get_shortcuts() const
    {
        return _internal->shortcuts;
    }

    Action::operator GAction*() const
    {
        return G_ACTION(_internal->g_action);
    }

    Action::operator GObject*() const
    {
        return G_OBJECT(_internal->g_action);
    }

    ActionID Action::get_id() const
    {
        return _internal->id;
    }

    void Action::set_enabled(bool b)
    {
        _internal->enabled = b;

        if (_internal->g_action != nullptr)
            g_simple_action_set_enabled(_internal->g_action, b);
    }

    bool Action::get_enabled() const
    {
        return _internal->enabled;
    }

    bool Action::get_is_stateful() const
    {
        return _internal->stateful_f != nullptr;
    }
}