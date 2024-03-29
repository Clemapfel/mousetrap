//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/13/23
//

#include <mousetrap/action.hpp>
#include <mousetrap/application.hpp>
#include <mousetrap/log.hpp>

#include <iostream>

namespace mousetrap::detail
{
    DECLARE_NEW_TYPE(ActionInternal, action_internal, ACTION_INTERNAL)
    DEFINE_NEW_TYPE_TRIVIAL_FINALIZE(ActionInternal, action_internal, ACTION_INTERNAL)
    DEFINE_NEW_TYPE_TRIVIAL_INIT(ActionInternal, action_internal, ACTION_INTERNAL)
    DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(ActionInternal, action_internal, ACTION_INTERNAL)

    static ActionInternal* action_internal_new(const std::string& in, Application& app)
    {
        auto* self = (ActionInternal*) g_object_new(action_internal_get_type(), nullptr);
        action_internal_init(self);

        self->id = in;
        self->application = (detail::ApplicationInternal*) app.get_internal();
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
    Action::Action(const std::string& id, Application& app)
        : Action(detail::action_internal_new(id, app))
    {}

    Action::Action(detail::ActionInternal* internal)
        : _internal(internal),
           CTOR_SIGNAL(Action, activated)
    {
        g_object_ref(_internal);
    }

    Action::~Action()
    {
        g_object_unref(_internal);
    }

    Action::Action(const Action& other)
        : Action(other._internal)
    {}

    Action::Action(Action&& other) noexcept
        : Action(other._internal)
    {}

    Action::operator NativeObject() const
    {
        return G_OBJECT(_internal->g_action);
    }

    NativeObject Action::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    void Action::update_application()
    {
        auto app = Application(_internal->application);
        app.remove_action(_internal->id);
        app.add_action(*this);
    }

    void Action::on_action_activate(GSimpleAction*, GVariant* variant, detail::ActionInternal* instance)
    {
        auto self = Action(instance);

        if (instance->stateless_f)
            instance->stateless_f(self);

        if (instance->stateful_f)
            instance->stateful_f(self);
    }

    void Action::on_action_change_state(GSimpleAction*, GVariant* variant, detail::ActionInternal* instance)
    {
        auto self = Action(instance);

        if (instance->stateless_f)
            instance->stateless_f(self);

        if (instance->stateful_f)
            instance->stateful_f(self);
    }

    void Action::activate()
    {
        g_action_activate(G_ACTION(_internal->g_action), nullptr);
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
            str << "In Action::add_shortcut: Unable to parse shortcut trigger `" << shortcut << "`";
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            return;
        }

        g_object_unref(trigger);
        _internal->shortcuts.push_back(shortcut.c_str());
        update_application();
    }

    const std::vector<ShortcutTriggerID>& Action::get_shortcuts() const
    {
        return _internal->shortcuts;
    }

    void Action::clear_shortcuts()
    {
        _internal->shortcuts.clear();
        update_application();
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