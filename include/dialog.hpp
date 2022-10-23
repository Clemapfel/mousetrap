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

            template<typename Function_t, typename Data_t>
            void add_action_button(const std::string&, Function_t, Data_t);

            template<typename Function_t, typename Data_t>
            void add_action_widget(Widget*, Function_t, Data_t);

            Box& get_content_area();
            void close();

        private:
            Box _content_area;
            Box _action_area;
            std::vector<Button*> _buttons;

            std::unordered_map<size_t, std::function<void()>> _actions;

            static void on_response_wrapper(GtkDialog*, int response_id, Dialog* instance);
    };
}

#include <src/dialog.inl>
