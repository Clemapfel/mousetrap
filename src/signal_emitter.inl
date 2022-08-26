// 
// Copyright 2022 Clemens Cords
// Created on 8/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

namespace mousetrap
{
    void SignalEmitter::set_signal_blocked(const std::string& signal_id, bool b)
    {
        auto it = _signal_handlers.find(signal_id);

        if (it == _signal_handlers.end())
        {
            std::cerr << "[WARNING] In Widget::set_signal_blocked: no signal with id \"" << signal_id << "\" connected."
                      << std::endl;
            return;
        }

        if (b)
            g_signal_handler_block(operator GObject*(), it->second);
        else
            g_signal_handler_unblock(operator GObject*(), it->second);
    }

    void SignalEmitter::set_all_signals_blocked(bool b)
    {
        for (auto& pair: _signal_handlers)
        {
            if (b)
                g_signal_handler_block(operator GObject*(), pair.second);
            else
                g_signal_handler_unblock(operator GObject*(), pair.second);
        }
    }

    template<typename Function_t>
    void SignalEmitter::connect_signal(const std::string& signal_id, Function_t* function, void* data)
    {
        auto handler_id = g_signal_connect(operator GObject*(), signal_id.c_str(), G_CALLBACK(function), data);
        _signal_handlers.insert_or_assign(signal_id, handler_id);
    }
}