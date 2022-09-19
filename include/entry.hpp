// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class Entry : public WidgetImplementation<GtkEntry>
    {
        public:
            Entry();

            std::string get_text() const;
            void set_text(const std::string&);

            void set_n_chars(size_t);
            void set_has_frame(bool);

            template<typename T>
            using on_activate_function_t = void(Entry*, T);

            template<typename Function_t, typename T>
            void connect_signal_activate(Function_t, T);

        private:
            static void on_activate_wrapper(GtkEntry*, Entry* instance);

            std::function<on_activate_function_t<void*>> _on_activate_f;
            void* _on_activate_data;
    };
}

#include <src/entry.inl>
