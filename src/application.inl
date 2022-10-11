// 
// Copyright 2022 Clemens Cords
// Created on 8/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <fstream>

namespace mousetrap
{
    Application::Application()
    {
        _native = gtk_application_new(nullptr, G_APPLICATION_FLAGS_NONE);
        _native = g_object_ref(_native);
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

    void Application::action_wrapper(GSimpleAction*, GVariant*, action_wrapper_data* data)
    {
        auto* action = data->instance->_actions.at(data->id);
        action->function(action->data);
    }

    template<typename Function_t, typename Data_t>
    void Application::add_action(const std::string& id, Function_t f, Data_t user_data)
    {
        auto temp = std::function<void(Data_t)>(f);
        auto final_id = "app." + id;
        auto inserted = _actions.insert({final_id, new action_data{
            std::function<void(void*)>(*(reinterpret_cast<std::function<void(void*)>*>(&temp))),
            reinterpret_cast<void*>(user_data)
        }}).first->second;

        auto* action = g_simple_action_new(id.c_str(), nullptr);
        g_signal_connect(G_OBJECT(action), "activate", G_CALLBACK(action_wrapper), (void*) (new action_wrapper_data{final_id, this}));
        g_action_map_add_action(G_ACTION_MAP(_native), G_ACTION(action));
    }

    template<typename Function_t>
    void Application::add_stateful_action(const std::string& id, Function_t f, bool* state)
    {
        auto* action = g_property_action_new(id.c_str(), state, "state");
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

    void Application::set_menubar(MenuModel* model)
    {
        gtk_application_set_menubar(_native, model->operator GMenuModel*());
    }

    void Application::add_window(Window* window)
    {
        gtk_application_add_window(_native, window->operator _GtkWindow *());
    }

}
