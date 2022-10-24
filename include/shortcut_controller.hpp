// 
// Copyright 2022 Clemens Cords
// Created on 10/22/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/event_controller.hpp>
#include <include/action_map.hpp>

#include <iostream>

namespace mousetrap
{
    using ShortcutCode = std::string;
    class ShortcutController : public EventController
    {
        public:
            ShortcutController(ActionMap*);
            ~ShortcutController();

            /// \brief actions handled by self:
            void add_action(ActionID id);
            void remove_action(ActionID id);

        private:
            ActionMap* _action_map;
            std::unordered_map<ActionID, std::vector<GtkShortcut*>> _shortcuts;
    };
}

//

namespace mousetrap
{
    ShortcutController::ShortcutController(ActionMap* map)
        : EventController(gtk_shortcut_controller_new()), _action_map(map)
    {}

    ShortcutController::~ShortcutController()
    {
        for (auto& pair : _shortcuts)
            for (auto* shortcut : pair.second)
                g_object_unref(shortcut);
    }

    void ShortcutController::add_action(ActionID id)
    {
        auto* action = gtk_shortcut_action_parse_string(("app." + id).c_str());
        if (action == nullptr)
        {
            std::cerr << "[ERROR] In ShortcutController::add_action: No action with ID `" << id << "` registered." << std::endl;
            return;
        }

        for (auto& s : _action_map->get_action(id).get_shortcuts())
        {
            auto* trigger = gtk_shortcut_trigger_parse_string(s.c_str());

            if (_shortcuts.find(id) == _shortcuts.end())
                _shortcuts.insert({id, {}});

            auto& shortcuts = _shortcuts.at(id);
            shortcuts.push_back(g_object_ref(gtk_shortcut_new(trigger, action)));
            gtk_shortcut_controller_add_shortcut(GTK_SHORTCUT_CONTROLLER(_native), shortcuts.back());
        }
    }

    void ShortcutController::remove_action(ActionID id)
    {
        auto* action = gtk_shortcut_action_parse_string(("app." + id).c_str());

        auto it = _shortcuts.find(id);
        if (it == _shortcuts.end())
        {
            std::cerr << "[WARNING] In ShortcutController::remove_action: No actino with ID `" << id << "` registered." << std::endl;
            return;
        }

        for (auto* shortcut : it->second)
        {
            gtk_shortcut_controller_remove_shortcut(GTK_SHORTCUT_CONTROLLER(_native), shortcut);
            g_object_unref(shortcut);
        }

        _shortcuts.erase(it);
    }
}