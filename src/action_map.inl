// 
// Copyright 2022 Clemens Cords
// Created on 10/26/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Action::Action(const std::string& id)
            : _id(id)
    {}

    Action::~Action()
    {}

    template<typename DoFunction_t, typename DoData_t>
    void Action::set_do_function(DoFunction_t f_in, DoData_t data_in)
    {
        _do = [f = f_in, data = data_in]() {
            f(data);
        };

        _g_action = g_object_ref(g_simple_action_new(_id.c_str(), nullptr));
        g_signal_connect(G_OBJECT(_g_action), "activate", G_CALLBACK(on_action_activate), this);
    }

    template<typename DoFunction_t>
    void Action::set_do_function(DoFunction_t f_in)
    {
        _do = [f = f_in]() {
            f();
        };

        _g_action = g_object_ref(g_simple_action_new(_id.c_str(), nullptr));
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

        if (accels.size() != 0)
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