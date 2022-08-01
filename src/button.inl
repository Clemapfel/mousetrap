// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Button::Button()
    {
        _native = GTK_BUTTON(gtk_button_new());
        connect_signal("clicked", on_clicked_wrapper);
    }

    Button::~Button()
    {
        //gtk_widget_destroy(GTK_WIDGET(_native));

        //if (_icon != nullptr)
            //gtk_widget_destroy(GTK_WIDGET(_icon));
    }

    GtkWidget* Button::get_native()
    {
        return GTK_WIDGET(_native);
    }

    void Button::set_label(const std::string& text)
    {
        gtk_button_set_label(_native, text.c_str());
    }

    void Button::set_icon(const std::string& path)
    {
        _icon = GTK_IMAGE(gtk_image_new_from_file(path.c_str()));
        gtk_button_set_image(_native, GTK_WIDGET(_icon));
    }

    void Button::set_relief_active(bool b)
    {
        if (b)
            gtk_button_set_relief(_native, GTK_RELIEF_NONE);
        else
            gtk_button_set_relief(_native, GTK_RELIEF_NORMAL);
    }

    void Button::on_clicked(GtkButton* self, gpointer user_data)
    {}

    void Button::on_clicked_wrapper(GtkButton* self, void* instance)
    {
        ((Button*) instance)->on_clicked(self, nullptr);
    }
}