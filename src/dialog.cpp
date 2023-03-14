// 
// Copyright 2022 Clemens Cords
// Created on 10/7/22 by clem (mail@clemens-cords.com)
//

#include <include/dialog.hpp>

namespace mousetrap
{
    Dialog::Dialog(Window* host_window, const std::string& title, bool is_modal)
            : WidgetImplementation<GtkDialog>(GTK_DIALOG(gtk_dialog_new_with_buttons(
                    title.c_str(),
                    host_window == nullptr ? nullptr : host_window->operator GtkWindow*(),
                    static_cast<GtkDialogFlags>(GTK_DIALOG_DESTROY_WITH_PARENT | (is_modal ? GTK_DIALOG_MODAL : 0)),
                    nullptr, GTK_RESPONSE_NONE, nullptr))),
              HasCloseSignal<Dialog>(this),
              _content_area(GTK_BOX(gtk_dialog_get_content_area(get_native()))),
              _action_area(GTK_ORIENTATION_HORIZONTAL)
    {
        g_signal_connect(get_native(), "response", G_CALLBACK(on_response_wrapper), this);
        gtk_window_set_hide_on_close(GTK_WINDOW(get_native()), true);
    }

    Dialog::~Dialog()
    {}

    void Dialog::close()
    {
        gtk_window_close(GTK_WINDOW(get_native()));
        emit_signal_close();
    }

    void Dialog::present()
    {
        gtk_window_present(GTK_WINDOW(get_native()));
    }

    Box& Dialog::get_content_area()
    {
        return _content_area;
    }

    Dialog::operator Window()
    {
        return Window(GTK_WINDOW(get_native()));
    }

    void Dialog::on_response_wrapper(GtkDialog*, int response_id, Dialog* instance)
    {
        if (response_id == GTK_RESPONSE_CLOSE)
            instance->emit_signal_close();

        if (response_id >= 0)
            instance->_actions.at(response_id)();
    }
}