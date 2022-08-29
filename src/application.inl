// 
// Copyright 2022 Clemens Cords
// Created on 8/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

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

    void Application::add_action(const std::string& id, ActionSignature f, void* user_data)
    {
        auto* action = g_simple_action_new(id.c_str(), nullptr);
        g_signal_connect(G_OBJECT(action), "activate", G_CALLBACK(f), user_data);
        g_action_map_add_action(G_ACTION_MAP(_native), G_ACTION(action));
    }
}
