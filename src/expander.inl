// 
// Copyright 2022 Clemens Cords
// Created on 9/18/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Expander::Expander(const std::string& title)
        : WidgetImplementation<GtkExpander>(GTK_EXPANDER(gtk_expander_new(title.c_str())))
    {}

    void Expander::set_child(Widget* widget)
    {
        gtk_expander_set_child(get_native(), widget->operator GtkWidget*());
    }

    bool Expander::get_expanded()
    {
        gtk_expander_get_expanded(get_native());
    }

    void Expander::set_expanded(bool b)
    {
        gtk_expander_set_expanded(get_native(), b);
    }

    void Expander::set_label_widget(Widget* widget)
    {
        gtk_expander_set_label_widget(get_native(), widget->operator GtkWidget*());
    }

    void Expander::set_resize_toplevel(bool b)
    {
        gtk_expander_set_resize_toplevel(get_native(), b);
    }
}