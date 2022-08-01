// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

#include <map>
#include <string>

namespace mousetrap
{
    class Widget
    {
        public:
            virtual GtkWidget* get_native() = 0;
            operator GtkWidget*();

            Vector2f get_size_request();

            void set_signal_blocked(const std::string& signal_id, bool);
            void set_all_signals_blocked(bool);

            template<typename Function_t>
            void connect_signal(const std::string& signal_id, Function_t*, void* data = nullptr);

        private:
            std::map<std::string, size_t> _signal_handlers;
    };
};

#include <src/widget.inl>
