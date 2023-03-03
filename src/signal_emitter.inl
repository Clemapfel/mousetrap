// 
// Copyright 2022 Clemens Cords
// Created on 8/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <iostream>

namespace mousetrap
{
    inline void SignalEmitter::set_signal_blocked(const std::string& signal_id, bool b)
    {
        auto it = _signal_handlers.find(signal_id);

        if (it == _signal_handlers.end())
        {
            std::cerr << "[WARNING] In Widget::set_signal_blocked: no signal with id \"" << signal_id << "\" connected."
                      << std::endl;
            return;
        }

        if (b)
        {
            if (not it->second.is_blocked)
            {
                g_signal_handler_block(operator GObject*(), it->second.id);
                it->second.is_blocked = true;
            }
        }
        else
        {
            if (it->second.is_blocked)
            {
                g_signal_handler_unblock(operator GObject*(), it->second.id);
                it->second.is_blocked = false;
            }
        }
    }

    inline void SignalEmitter::set_all_signals_blocked(bool b)
    {
        for (auto& pair: _signal_handlers)
            set_signal_blocked(pair.first, b);
    }

    template<typename Function_t>
    void SignalEmitter::connect_signal(const std::string& signal_id, Function_t* function, void* data)
    {
        auto handler_id = g_signal_connect(operator GObject*(), signal_id.c_str(), G_CALLBACK(function), data);
        _signal_handlers.insert_or_assign(signal_id, SignalHandler{handler_id});
    }

    inline std::vector<std::string>SignalEmitter::get_all_signal_names()
    {
        std::vector<std::string> out;
        guint n;
        auto* ids = g_signal_list_ids(gtk_file_chooser_get_type(), &n);
        for (size_t i = 0; i < n; ++i)
            out.emplace_back(g_signal_name(ids[i]));

        g_free(ids);
        return out;
    }
}