// 
// Copyright 2022 Clemens Cords
// Created on 10/7/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/window.hpp>

namespace mousetrap
{
    class Dialog : public WidgetImplementation<GtkDialog>, public HasCloseSignal<Dialog>
    {
        public:
            Dialog(Window* host_window, const std::string& title = "", bool is_modal = true);
            ~Dialog();

            template<typename Function_t, typename T>
            void add_action_button(const std::string&, Function_t, T);

            template<typename Function_t, typename T>
            void add_action_widget(Widget*, Function_t, T);

            Box& get_content_area();
            void close();

        private:
            Box _content_area;

            using function_data = struct {std::function<void(void*)> function; void* data;};
            std::unordered_map<size_t, function_data*> _actions;

            static void on_response_wrapper(GtkDialog*, int response_id, Dialog* instance);
    };
}

#include <src/dialog.inl>
