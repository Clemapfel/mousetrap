//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/13/23
//

namespace mousetrap
{
    Action::Action(const std::string& id)
        : _id(id)
    {}

    Action::~Action()
    {
        //g_object_unref(_g_action);
    }

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
    }

    void Action::on_action_activate(GSimpleAction*, GVariant*, Action* instance)
    {
        instance->activate();
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
}