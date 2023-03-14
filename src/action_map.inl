// 
// Copyright 2022 Clemens Cords
// Created on 10/26/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    inline void ActionMap::add_action(Action& action)
    {
        auto inserted = _actions.insert({action.get_id(), action}).first->second;
        auto* self = operator GActionMap*();
        g_action_map_add_action(self, inserted.operator GAction *());

        auto* app = operator GtkApplication*();

        auto accels = std::vector<const char*>();
        for (auto& s : inserted.get_shortcuts())
        {
            if (s != "never")
                accels.push_back(s.c_str());
        }
        accels.push_back(NULL);

        if (not accels.empty())
            gtk_application_set_accels_for_action(app, ("app." + inserted.get_id()).c_str(), accels.data());
    }

    inline void ActionMap::remove_action(const ActionID& id)
    {
        auto* self = operator GActionMap*();
        _actions.erase(id);
        g_action_map_remove_action(self, ("app." + id).c_str());
    }

    inline bool ActionMap::has_action(const ActionID& id)
    {
        return _actions.find(id) != _actions.end();
    }

    inline const Action& ActionMap::get_action(const ActionID& id)
    {
        return _actions.at(id);
    }
}