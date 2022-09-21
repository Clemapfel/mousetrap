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

            using ActionSignature = void(*)(void* user_data);

            template<typename T>
            void add_action(const std::string& id, ActionSignature, T user_data);
            void activate_action(const std::string& id);
            GAction* get_action(const std::string& id);

        private:
            static void action_wrapper(GSimpleAction*, GVariant*, std::pair<ActionSignature, void*>*);

            GtkApplication* _native;
    };
}

#include <src/application.inl>