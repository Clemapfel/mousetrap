// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class Button : public WidgetImplementation<GtkButton>
    {
        public:
            Button();

            void set_has_frame(bool b);
            void set_child(Widget*);

            template<typename T>
            using on_clicked_function_t = void(Button*, T);

            template<typename Function_t, typename T>
            void connect_signal_clicked(Function_t function, T data);

        private:
            static void on_clicked_wrapper(GtkButton*, Button* instance);

            std::function<on_clicked_function_t<void*>> _on_clicked_f;
            void* _on_clicked_data;
    };
}

#include <src/button.inl>

namespace mousetrap
{









}