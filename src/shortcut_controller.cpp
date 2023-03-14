// 
// Copyright 2022 Clemens Cords
// Created on 10/26/22 by clem (mail@clemens-cords.com)
//

#include <include/shortcut_controller.hpp>

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
        auto* action = gtk_shortcut_action_parse_string(("action(" + ("app." + id) + ")").c_str());

        if (action == nullptr)
        {
            std::cerr << "[ERROR] In ShortcutController::add_action: No action with ID `" << id << "` registered." << std::endl;
            return;
        }

        if (not _action_map->has_action(id))
        {
            std::cerr << "[ERROR] In ShortcutController::add_action: No action with ID `" << id << "` registered."
                      << std::endl;
            return;
        }

        for (auto& s : _action_map->get_action(id).get_shortcuts())
        {
            auto* trigger = gtk_shortcut_trigger_parse_string(s.c_str());
            if (trigger == nullptr)
            {
                std::cerr << "[ERROR] In ShortcutController::add_action: Unable to parse trigger `" << s << "` for action `" << id << "`" << std::endl;
                return;
            }

            if (_shortcuts.find(id) == _shortcuts.end())
                _shortcuts.insert({id, {}});

            auto& shortcuts = _shortcuts.at(id);
            shortcuts.push_back(g_object_ref(gtk_shortcut_new(trigger, action)));
            gtk_shortcut_controller_add_shortcut(GTK_SHORTCUT_CONTROLLER(_native), shortcuts.back());
        }
    }

    void ShortcutController::remove_action(ActionID id)
    {
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

    void ShortcutController::set_scope(ShortcutScope scope)
    {
        gtk_shortcut_controller_set_scope(GTK_SHORTCUT_CONTROLLER(_native), (GtkShortcutScope) scope);
    }

    ShortcutScope ShortcutController::get_scope()
    {
        return (ShortcutScope) gtk_shortcut_controller_get_scope(GTK_SHORTCUT_CONTROLLER(_native));
    }
}