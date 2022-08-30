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
        public:
            ShortcutMap() = default;

            void load_from_file(const std::string&);

            /// \brief does current event fit shortcut
            bool should_trigger(GdkEvent* event, const std::string& action_id);

        private:
            std::map<std::string, GtkShortcutTrigger*> _bindings;
    };
}

// ###

#include <fstream>
#include <iostream>

namespace mousetrap
{
    void ShortcutMap::load_from_file(const std::string& config_file)
    {
        std::ifstream file;
        file.open(config_file);

        std::string line;
        size_t line_i = 0;

        std::string current_region = "";
        while (getline(file, line))
        {
            std::string action_name, trigger_name;
            size_t i = 0;
            bool action_active = true;

            // comment or empty line
            if (line.size() == 0 or line.at(0) == '#')
                continue;

            // region marker
            if (line.at(0) == '[')
            {
                current_region = "";
                size_t i = 1;

                try
                {
                    while (line.at(i) != ']')
                    {
                        current_region.push_back(line.at(i));
                        i += 1;
                    }
                    continue;
                }
                catch (...)
                {
                    goto on_error;
                }
            }

            // assignment
            try
            {
                while (i < line.size())
                {
                    if (line.at(i) == ' ')
                    {
                        if (not action_active)
                            goto on_error;

                        if (line.at(i+1) == '=' and line.at(i+2) == ' ')
                        {
                            action_active = false;
                            i += 3;
                            continue;
                        }
                        else
                            goto on_error;
                    }

                    if (action_active)
                        action_name.push_back(line.at(i));
                    else
                        trigger_name.push_back(line.at(i));

                    i += 1;
                }
            }
            catch (...)
            {
                goto on_error;
            }

            _bindings.insert({
                current_region + "." + action_name,
                gtk_shortcut_trigger_parse_string(trigger_name.c_str())
            });

            line_i += 1;
            continue;

            on_error:
            {
                std::cerr << "In mousetrap::load_shortcuts: Error when parsing " << config_file << " at line " << line_i << ":\n"
                          << line << std::endl;
                return;
            };
        }
    }


    bool ShortcutMap::should_trigger(GdkEvent* event, const std::string& id)
    {
        auto it = _bindings.find(id);
        if (it == _bindings.end())
        {
            std::cerr << "[WARNING] In ShortcutBindingMap::should_trigger: No shortcut with id \"" << id << "\" registered." << std::endl;
            return false;
        }

        auto* trigger = it->second;

        if (trigger != nullptr)
            return gtk_shortcut_trigger_trigger(trigger, event, false);
    }
}