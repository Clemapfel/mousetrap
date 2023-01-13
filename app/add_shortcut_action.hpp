//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/13/23
//

#pragma once

#include <app/global_state.hpp>

namespace mousetrap::state
{
    void add_shortcut_action(Action& action)
    {
        auto id = action.get_id();

        std::stringstream group;
        std::stringstream key;

        // split action IDs, always has the from <group>.<key>
        bool group_active = true;
        for (auto c : id)
        {
            if (c == '.')
            {
                group_active = false;
                continue;
            }

            if (group_active)
                group << c;
            else
                key << c;
        }

        bool has_shortcut = state::keybindings_file->has_group(group.str()) && state::keybindings_file->has_key(group.str(), key.str());

        if (has_shortcut)
            action.add_shortcut(state::keybindings_file->get_value(group.str(), key.str()));

        state::app->add_action(action);

        if (has_shortcut)
            state::shortcut_controller->add_action(id);
    }
}
