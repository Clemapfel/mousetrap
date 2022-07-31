// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace rat
{
    class Button : public Widget
    {
        public:
            Button();
            ~Button();

            void set_label(const std::string&);
            void set_icon(const std::string& path);

            GtkWidget* get_native();

        protected:
            void on_clicked(GtkButton* self, gpointer user_data);

        private:
            static void on_clicked_wrapper(GtkButton* self, void* instance);
            GtkButton* _native;
            GtkImage* _icon;
    };
}

// ###

namespace rat
{
    Button::Button()
    {
        _native = GTK_BUTTON(gtk_button_new());
        connect_signal("clicked", on_clicked_wrapper);
    }

    Button::~Button()
    {
        gtk_widget_destroy(GTK_WIDGET(_native));

        if (_icon != nullptr)
            gtk_widget_destroy(GTK_WIDGET(_icon));
    }

    GtkWidget* Button::get_native()
    {
        return _native;
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

    void Button::on_clicked(GtkButton* self, gpointer user_data)
    {}

    void Button::on_clicked_wrapper(GtkButton* self, void* instance)
    {
        ((Button*) instance)->on_clicked(self, nullptr);
    }
}