// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class ToggleButton : public WidgetImplementation<GtkToggleButton>
    {
        public:
            ToggleButton();

            bool get_active() const;
            void set_active(bool b);

            void set_label(const std::string&);
            void set_child(Widget*);
            void set_has_frame(bool b);

            template<typename T>
            using on_toggled_function_t = void(ToggleButton*, T);

            template<typename Function_t, typename T>
            void connect_signal_toggled(Function_t function, T data);

        private:
            static void on_toggled_wrapper(GtkToggleButton*, ToggleButton* instance);

            std::function<on_toggled_function_t<void*>> _on_toggled_f;
            void* _on_toggled_data;
    };
}

#include <src/toggle_button.inl>