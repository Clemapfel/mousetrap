// 
// Copyright 2022 Clemens Cords
// Created on 8/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <fstream>

namespace mousetrap
{
    template<typename InitializeFunction_t>
    Application::Application(InitializeFunction_t* initialize, void* data)
    {
        _native = gtk_application_new(nullptr, G_APPLICATION_FLAGS_NONE);
        g_signal_connect(_native, "activate", G_CALLBACK(initialize), data);
    }

    Application::~Application()
    {
        g_object_unref(_native);
    }

    int Application::run()
    {
        return g_application_run(G_APPLICATION(_native), 0, nullptr);
    }

    Application::operator GObject*()
    {
        return G_OBJECT(_native);
    }

    void Application::action_wrapper(GSimpleAction*, GVariant*, std::pair<ActionSignature, void*>* data)
    {
        (*data->first)(data->second);
    }

    void Application::add_action(const std::string& id, ActionSignature f, void* user_data)
    {
        auto* action = g_simple_action_new(id.c_str(), nullptr);
        g_signal_connect(G_OBJECT(action), "activate", G_CALLBACK(action_wrapper), (void*) (new std::pair<ActionSignature, void*>(f, user_data)));
        g_action_map_add_action(G_ACTION_MAP(_native), G_ACTION(action));
    }

    void Application::activate_action(const std::string& id)
    {
        auto* action = g_action_map_lookup_action(G_ACTION_MAP(_native), id.c_str());
        if (action == nullptr)
        {
            std::cerr << "[ERROR] In Application::trigger_action: No action with id \"" << id << "\"." << std::endl;
            return;
        }
        g_action_activate(action, nullptr);
    }

    GAction* Application::get_action(const std::string& id)
    {
        return g_action_map_lookup_action(G_ACTION_MAP(_native), id.c_str());
    }

    /*
    void Application::add_shortcut(const std::string& region, GtkShortcut* shortcut)
    {
        auto it = _shortcuts.find(region);

        if (it == _shortcuts.end())
            _shortcuts.insert({region, {}});

        _shortcuts.at(region).push_back(shortcut);
    }

    void Application::load_shortcuts(const std::string& config_file)
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

            add_shortcut(current_region, gtk_shortcut_new(
                gtk_shortcut_trigger_parse_string(trigger_name.c_str()),
                gtk_shortcut_action_parse_string((current_region + "." + action_name).c_str())
            ));

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

    std::vector<GtkShortcut*>& Application::get_shortcuts(const std::string& region)
    {
        static std::vector<GtkShortcut*> on_error = {};

        auto it = _shortcuts.find(region);
        if (it == _shortcuts.end())
        {
            std::cerr << "[WARNING] In Application::get_shortcuts: No shortcut region named \"" << region << "\" registered." << std::endl;
            return on_error;
        }
        else
            return it->second;
    }
     */
}
