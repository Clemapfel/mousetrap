// 
// Copyright 2022 Clemens Cords
// Created on 8/25/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

#include <string>

namespace mousetrap
{
    class Application
    {
        public:
            template<typename InitializeFunction_t>
            Application(InitializeFunction_t*, void* = nullptr);

            virtual ~Application();

            int run();

        private:
            GtkApplication* _native;
    };
}

// ###

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
}