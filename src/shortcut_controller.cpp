//
// Copyright 2022 Clemens Cords
// Created on 10/26/22 by clem (mail@clemens-cords.com)
//

#include <include/shortcut_controller.hpp>

namespace mousetrap
{
    ShortcutController::ShortcutController()
        : EventController(gtk_shortcut_controller_new())
    {}

    void ShortcutController::add_action(const Action& action)
    {
        auto id = action.get_id();
        auto* shortcut_action = gtk_shortcut_action_parse_string(("action(" + ("app." + id) + ")").c_str());

        for (auto& s : action.get_shortcuts())
        {
            auto* trigger = gtk_shortcut_trigger_parse_string(s.c_str());
            if (trigger == nullptr)
            {
                std::cerr << "[ERROR] In ShortcutController::add_action: Unable to parse trigger `" << s << "` for action `" << id << "`" << std::endl;
                return;
            }
            gtk_shortcut_controller_add_shortcut(GTK_SHORTCUT_CONTROLLER(_native), gtk_shortcut_new(trigger, shortcut_action));
        }
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