// 
// Copyright 2022 Clemens Cords
// Created on 8/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <fstream>

namespace mousetrap
{
    inline Application::Application()
    {
        _native = gtk_application_new(nullptr, G_APPLICATION_DEFAULT_FLAGS);
        _native = g_object_ref(_native);
    }

    inline Application::~Application()
    {
        g_object_unref(_native);
    }

    inline int Application::run()
    {
        return g_application_run(G_APPLICATION(_native), 0, nullptr);
    }

    inline Application::operator GObject*()
    {
        return G_OBJECT(_native);
    }

    inline Application::operator GtkApplication*()
    {
        return _native;
    }

    inline Application::operator GActionMap*()
    {
        return G_ACTION_MAP(_native);
    }

    inline void Application::set_menubar(MenuModel* model)
    {
        gtk_application_set_menubar(_native, model->operator GMenuModel*());
    }

    inline void Application::add_window(Window* window)
    {
        gtk_application_add_window(_native, window->operator  GtkWindow*());
    }
}
