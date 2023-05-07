// 
// Copyright 2022 Clemens Cords
// Created on 8/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

#include <string>
#include <map>
#include <vector>

namespace mousetrap
{
    class SignalEmitter
    {
        public:
            void set_signal_blocked(const std::string& signal_id, bool);
            void set_all_signals_blocked(bool);

            template<typename Function_t>
            void connect_signal(const std::string& signal_id, Function_t*, void* data = nullptr);

            std::vector<std::string> get_all_signal_names();
            virtual operator GObject*() = 0;

        private:
            struct SignalHandler
            {
                size_t id;
                bool is_blocked = false;
            };

            std::map<std::string, SignalHandler> _signal_handlers = {};
    };
}

namespace mousetrap
{
    template<typename Function_t>
    void SignalEmitter::connect_signal(const std::string& signal_id, Function_t* function, void* data)
    {
        auto handler_id = g_signal_connect(operator GObject*(), signal_id.c_str(), G_CALLBACK(function), data);
        _signal_handlers.insert_or_assign(signal_id, SignalHandler{handler_id});
    }
}