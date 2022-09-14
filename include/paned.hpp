// 
// Copyright 2022 Clemens Cords
// Created on 9/14/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class Paned : public Widget
    {
        public:
            Paned(GtkOrientation);
            operator GtkWidget*() override;

            int get_position();
            void set_position(int);

            void set_start_child(Widget*);
            void set_start_child_resizable(bool);
            void set_start_child_shrinkable(bool);

            void set_end_child(Widget*);
            void set_end_child_resizable(bool);
            void set_end_child_shrinkable(bool);

            void set_has_wide_handle(bool);

        private:
            GtkPaned* _native;
    };
}

// #include <include/paned.hpp>

namespace mousetrap
{
    Paned::Paned(GtkOrientation orientation)
    {
        _native = GTK_PANED(gtk_paned_new(orientation));
    }

    Paned::operator GtkWidget*()
    {
        return GTK_WIDGET(_native);
    }

    void Paned::set_position(int v)
    {
        gtk_paned_set_position(_native, v);
    }

    int Paned::get_position()
    {
        return gtk_paned_get_position(_native);
    }

    void Paned::set_start_child(Widget* widget)
    {
        gtk_paned_set_start_child(_native, widget->operator GtkWidget*());
    }

    void Paned::set_start_child_resizable(bool b)
    {
        gtk_paned_set_resize_start_child(_native, b);
    }

    void Paned::set_start_child_shrinkable(bool b)
    {
        gtk_paned_set_shrink_start_child(_native, b);
    }

    void Paned::set_end_child(Widget* widget)
    {
        gtk_paned_set_end_child(_native, widget->operator GtkWidget*());
    }

    void Paned::set_end_child_resizable(bool b)
    {
        gtk_paned_set_resize_end_child(_native, b);
    }

    void Paned::set_end_child_shrinkable(bool b)
    {
        gtk_paned_set_shrink_end_child(_native, b);
    }

    void Paned::set_has_wide_handle(bool b)
    {
        gtk_paned_set_wide_handle(_native, b);
    }
}