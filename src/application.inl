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
}
