// 
// Copyright 2022 Clemens Cords
// Created on 8/25/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

#include <include/signal_emitter.hpp>
#include <include/menu_model.hpp>
#include <include/action_map.hpp>

#include <string>

namespace mousetrap
{
    class Application : public SignalEmitter, public ActionMap
    {
        public:
            Application();
            virtual ~Application();

            int run();
            operator GObject*() override;
            operator GtkApplication*() override;
            operator GActionMap*() override;

            void set_menubar(MenuModel*);
            void add_window(Window*);

        private:
            GtkApplication* _native;
    };
}

#include <src/application.inl>