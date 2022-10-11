// 
// Copyright 2022 Clemens Cords
// Created on 8/25/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

#include <include/signal_emitter.hpp>
#include <include/menu_model.hpp>

#include <string>

namespace mousetrap
{
    class Application : public SignalEmitter
    {
        public:
            Application();

            virtual ~Application();

            int run();
            operator GObject*() override;

            using ActionSignature = void(*)(void* user_data);

            template<typename Function_t, typename Data_t>
            void add_action(const std::string& id, Function_t, Data_t user_data);

            void activate_action(const std::string& id);
            GAction* get_action(const std::string& id);

            void set_menubar(MenuModel*);
            void add_window(Window*);

        private:
            static void action_wrapper(GSimpleAction*, GVariant*, std::pair<ActionSignature, void*>*);

            GtkApplication* _native;
    };
}

#include <src/application.inl>