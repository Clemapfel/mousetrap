// 
// Copyright 2022 Clemens Cords
// Created on 9/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class Expander : public WidgetImplementation<GtkExpander>
    {
        public:
            Expander(const std::string&);

            void set_child(Widget*);

            void set_label_widget(Widget*);
            void set_resize_toplevel(bool);

            bool get_expanded();
            void set_expanded(bool);

            template<typename T>
            using on_activate_function_t = void(Expander*, T);
            
            template<typename Function_t, typename T>
            void connect_signal_activate(Function_t, T);
            
        private:
            static void on_activate_wrapper(GtkExpander*, Expander* instance);

            std::function<on_activate_function_t<void*>> _on_activate_f;
            void* _on_activate_data;
    };
}

#include <src/expander.inl>
