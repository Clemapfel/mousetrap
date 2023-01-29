//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/13/23
//

namespace mousetrap
{
    inline  Action::Action(const std::string& id)
        : _id(id)
    {}

    inline Action::Action(const std::string& id, bool* state)
        : _id(id)
    {
        set_function([](){
            std::cout << "called" << std::endl;
        });

        auto* variant = g_variant_new_boolean(false);
        _g_action = g_object_ref(g_simple_action_new_stateful(_id.c_str(), NULL, variant));
        g_signal_connect(G_OBJECT(_g_action), "activate", G_CALLBACK(on_action_activate), this);
        set_enabled(_enabled);
    }

    inline Action::~Action()
    {}

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

    inline void Action::on_action_activate(GSimpleAction*, GVariant*, Action* instance)
    {
        instance->activate();
    }

    inline void Action::activate() const
    {
        auto& state = _instance_states.at(_id);
        if (state->do_f)
            (*state->do_f)();
    }

    inline void Action::undo() const
    {
        auto& state = _instance_states.at(_id);
        if (not state->undo_queue->empty())
        {
            state->undo_queue->back()();
            state->undo_queue->pop_back();
        }
    }

    inline void Action::redo() const
    {
        auto& state = _instance_states.at(_id);
        if (not state->redo_queue->empty())
        {
            state->redo_queue->back()();
            state->redo_queue->pop_back();
        }
    }

    inline void Action::add_shortcut(const ShortcutTriggerID& shortcut)
    {
        if (gtk_shortcut_trigger_parse_string(shortcut.c_str()) == nullptr)
        {
            std::cerr << "[WARNING] In Action::add_shortcut: Unable to parse shortcut trigger `" << shortcut
                      << "`. Ignoring this shortcut binding" << std::endl;
            return;
        }

        _shortcuts.push_back(shortcut.c_str());
    }

    inline const std::vector<ShortcutTriggerID>& Action::get_shortcuts() const
    {
        return _shortcuts;
    }

    inline Action::operator GAction*() const
    {
        return G_ACTION(_g_action);
    }

    inline ActionID Action::get_id() const
    {
        return _id;
    }

    inline void Action::set_enabled(bool b)
    {
        _enabled = b;

        if (_g_action != nullptr)
            g_simple_action_set_enabled(_g_action, b);
    }

    inline bool Action::get_enabled() const
    {
        return _enabled;
    }
}