// 
// Copyright 2022 Clemens Cords
// Created on 9/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class Expander : public Widget
    {
        public:
            Expander(const std::string&);
            operator GtkWidget*() override;

            void set_child(Widget*);

            void set_label_widget(Widget*);
            void set_resize_toplevel(bool);

            bool get_expanded();
            void set_expanded(bool);

        private:
            GtkExpander* _native;
    };
}

// ###

namespace mousetrap
{
    Expander::Expander(const std::string& title)
    {
        _native = GTK_EXPANDER(gtk_expander_new(title.c_str()));
    }

    Expander::operator GtkWidget*()
    {
        return GTK_WIDGET(_native);
    }

    void Expander::set_child(Widget* widget)
    {
        gtk_expander_set_child(_native, widget->operator GtkWidget*());
    }

    bool Expander::get_expanded()
    {
        gtk_expander_get_expanded(_native);
    }

    void Expander::set_expanded(bool b)
    {
        gtk_expander_set_expanded(_native, b);
    }

    void Expander::set_label_widget(Widget* widget)
    {
        gtk_expander_set_label_widget(_native, widget->operator GtkWidget*());
    }

    void Expander::set_resize_toplevel(bool b)
    {
        gtk_expander_set_resize_toplevel(_native, b);
    }
}