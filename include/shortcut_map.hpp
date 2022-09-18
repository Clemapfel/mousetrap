// 
// Copyright 2022 Clemens Cords
// Created on 8/30/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

#include <string>
#include <map>

namespace mousetrap
{
    class ShortcutMap
    {
        friend class Window;

        public:
            ShortcutMap() = default;

            void load_from_file(const std::string&);

            /// \brief does current event fit shortcut
            bool should_trigger(GdkEvent* event, const std::string& action_id);

            /// \param action_prefix: "palette_view" will query all "palette_view.*" actions
            /// \note description for each binding will be generated from the variable name of the action in the .conf file
            std::string generate_control_tooltip(const std::string& action_prefix, const std::string& description_optional = "");

            std::string get_shortcut_as_string(const std::string& action_prefix, const std::string& action);
            GtkShortcutTrigger* get_shortcut(const std::string& action_prefix, const std::string& action);

        protected:
            static std::string trigger_to_string(GtkShortcutTrigger*);
            std::map<std::string, GtkShortcutTrigger*> _bindings;
    };
}

#include <src/shortcut_map.inl>