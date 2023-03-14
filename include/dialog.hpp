// 
// Copyright 2022 Clemens Cords
// Created on 10/7/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/window.hpp>
#include <include/box.hpp>
#include <include/button.hpp>

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

            void present();
            void close();

            operator Window();

        private:
            Box _content_area;
            Box _action_area;
            std::vector<Button*> _buttons;

            std::unordered_map<size_t, std::function<void()>> _actions;

            static void on_response_wrapper(GtkDialog*, int response_id, Dialog* instance);
    };
}

namespace mousetrap
{
    template<typename Function_t, typename T>
    void Dialog::add_action_button(const std::string& text, Function_t f_in, T data_in)
    {
        gtk_dialog_add_button(get_native(), text.c_str(), _actions.size());
        _actions.insert({_actions.size(), [f = f_in, data = data_in](){
            f(data);
        }});

        auto* button = gtk_dialog_get_widget_for_response(get_native(), _actions.size()-1);
        auto* action_area = GTK_BOX(gtk_widget_get_parent(button));
        gtk_box_set_homogeneous(action_area, true);
        gtk_box_set_spacing(action_area, 10);
        gtk_widget_set_halign(GTK_WIDGET(action_area), GTK_ALIGN_END);
    }

    template<typename Function_t, typename T>
    void Dialog::add_action_widget(Widget* widget, Function_t f_in, T data_in)
    {
        gtk_dialog_add_action_widget(get_native(), widget->operator GtkWidget *(), _actions.size());
        _actions.insert({_actions.size(), [f = f_in, data = data_in](){
            f(data);
        }});
    }
}
