// 
// Copyright 2022 Clemens Cords
// Created on 10/7/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Dialog::Dialog(Window* host_window, const std::string& title, bool is_modal)
            : WidgetImplementation<GtkDialog>(GTK_DIALOG(gtk_dialog_new_with_buttons(
                    title.c_str(),
                    host_window->operator GtkWindow*(),
                    static_cast<GtkDialogFlags>(GTK_DIALOG_DESTROY_WITH_PARENT | (is_modal ? GTK_DIALOG_MODAL : 0)),
                    nullptr))),
              HasCloseSignal<Dialog>(this),
              _content_area(GTK_BOX(gtk_dialog_get_content_area(get_native())))
    {
        g_signal_connect(get_native(), "response", G_CALLBACK(on_response_wrapper), this);
    }

    Dialog::~Dialog() noexcept
    {
        for (auto& pair : _actions)
            delete pair.second;
    }

    void Dialog::close()
    {
        gtk_window_close(GTK_WINDOW(get_native()));
    }

    template<typename Function_t, typename T>
    void Dialog::add_action_button(const std::string& text, Function_t function, T data)
    {
        gtk_dialog_add_button(get_native(), text.c_str(), _actions.size());
        auto temp =  std::function<void(T)>(function);
        _actions.insert({_actions.size(), new function_data{
                std::function<void(void*)>(*((std::function<void(void*)>*) &temp)),
                (void*) data
        }});
    }

    template<typename Function_t, typename T>
    void Dialog::add_action_widget(Widget* widget, Function_t function, T data)
    {
        gtk_dialog_add_action_widget(get_native(), widget->operator GtkWidget *(), _actions.size());

        auto temp =  std::function<void(T)>(function);
        _actions.insert({_actions.size(), new function_data{
                std::function<void(void*)>(*((std::function<void(void*)>*) &temp)),
                (void*) data
        }});
    }

    Box& Dialog::get_content_area()
    {
        return _content_area;
    }

    void Dialog::on_response_wrapper(GtkDialog*, int response_id, Dialog* instance)
    {
        if (response_id >= 0)
            instance->_actions.at(response_id)->function(instance->_actions.at(response_id)->data);
    }
}