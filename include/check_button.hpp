// 
// Copyright 2022 Clemens Cords
// Created on 9/16/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class CheckButton : public WidgetImplementation<GtkCheckButton>
    {
        public:
            CheckButton();
            CheckButton(const std::string& label);

            bool get_is_checked();
            void set_is_checked(bool b);

            bool get_is_inconsistent();
            void set_is_inconsistent(bool b);

            template<typename T>
            using on_toggled_function_t = void(CheckButton*, T);

            template<typename Function_t, typename T>
            void connect_signal_toggled(Function_t function, T data);

        private:
            static void on_toggled_wrapper(GtkCheckButton*, CheckButton* instance);

            std::function<on_toggled_function_t<void*>> _on_toggled_f;
            void* _on_toggled_data;
    };
}

#include <src/check_button.inl>
