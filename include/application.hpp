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

            using ActionSignature = void(*)(GSimpleAction* self, GVariant*, void* user_data);
            void add_action(const std::string& id, ActionSignature, void* user_data);

            void load_shortcuts(const std::string& config_file);
            void add_shortcut(const std::string& region, GtkShortcut* shortcut);
            std::vector<GtkShortcut*>& get_shortcuts(const std::string& region);

        private:
            GtkApplication* _native;
            std::map<std::string, std::vector<GtkShortcut*>> _shortcuts;
    };
}

#include <src/application.inl>