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

            /// name should **not** have app. prefix, for example `test_action`, not `app.test_action`
            template<typename Function_t, typename Data_t>
            void add_action(const std::string& id, Function_t, Data_t);

            template<typename Function_t>
            void add_stateful_action(const std::string& id, Function_t, bool* state);
            
            void activate_action(const std::string& id);
            GAction* get_action(const std::string& id);

            void set_menubar(MenuModel*);
            void add_window(Window*);

        private:
            GtkApplication* _native;

            using action_data = struct {std::function<void(void*)> function; void* data;};
            using action_id = std::string;
            std::unordered_map<action_id, action_data*> _actions;

            using action_wrapper_data = struct {action_id id; Application* instance;};
            static void action_wrapper(GSimpleAction*, GVariant*, action_wrapper_data*);
    };
}

#include <src/application.inl>