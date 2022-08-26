// 
// Copyright 2022 Clemens Cords
// Created on 8/25/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

#include <include/signal_emitter.hpp>

#include <string>

namespace mousetrap
{
    class Application : public SignalEmitter
    {
        public:
            template<typename InitializeFunction_t>
            Application(InitializeFunction_t*, void* = nullptr);

            virtual ~Application();

            int run();

            operator GObject*() override;

        private:
            GtkApplication* _native;
    };
}

#include <src/application.inl>