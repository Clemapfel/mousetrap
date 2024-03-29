//
// Copyright 2022 Clemens Cords
// Created on 10/26/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/shortcut_event_controller.hpp>

namespace mousetrap
{
    ShortcutEventController::ShortcutEventController()
        : EventController(gtk_shortcut_controller_new())
    {}

    ShortcutEventController::ShortcutEventController(detail::ShortcutEventControllerInternal* internal)
        : EventController(internal)
    {}

    void ShortcutEventController::add_action(const Action& action)
    {
        auto id = action.get_id();
        auto* shortcut_action = gtk_shortcut_action_parse_string(("action(" + ("app." + id) + ")").c_str());

        for (auto& s : action.get_shortcuts())
        {
            auto* trigger = gtk_shortcut_trigger_parse_string(s.c_str());
            if (trigger == nullptr)
            {
                std::cerr << "[ERROR] In ShortcutEventController::add_action: Unable to parse trigger `" << s << "` for action `" << id << "`" << std::endl;
                return;
            }
            gtk_shortcut_controller_add_shortcut(GTK_SHORTCUT_CONTROLLER(get_internal()), gtk_shortcut_new(trigger, shortcut_action));
        }
    }

    void ShortcutEventController::remove_action(const Action& action)
    {
        auto id = action.get_id();
        auto* shortcut_action = gtk_shortcut_action_parse_string(("action(" + ("app." + id) + ")").c_str());

        for (auto& s : action.get_shortcuts())
        {
            auto* trigger = gtk_shortcut_trigger_parse_string(s.c_str());
            if (trigger == nullptr)
            {
                std::cerr << "[ERROR] In ShortcutEventController::remove_action: Unable to parse trigger `" << s << "` for action `" << id << "`" << std::endl;
                return;
            }
            gtk_shortcut_controller_remove_shortcut(GTK_SHORTCUT_CONTROLLER(get_internal()), gtk_shortcut_new(trigger, shortcut_action));
        }
    }

    void ShortcutEventController::set_scope(ShortcutScope scope)
    {
        gtk_shortcut_controller_set_scope(GTK_SHORTCUT_CONTROLLER(get_internal()), (GtkShortcutScope) scope);
    }

    ShortcutScope ShortcutEventController::get_scope()
    {
        return (ShortcutScope) gtk_shortcut_controller_get_scope(GTK_SHORTCUT_CONTROLLER(get_internal()));
    }
}