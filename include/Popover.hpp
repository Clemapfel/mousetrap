// 
// Copyright 2022 Clemens Cords
// Created on 9/3/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class Popover : public Widget
    {
        public:
            Popover();
            operator GtkWidget*() override;

            void popup();
            void popdown();
            void present();

            void set_child(Widget*);

            void set_relative_position(GtkPositionType);
            void set_has_arrow(bool);
            void set_autohide(bool);

            // TODO: void attach_to(Widget*);

        private:
            GtkPopover* _native;
    };
}

#include <src/popover.inl>

namespace mousetrap
{
    Popover::Popover()
    {
        _native = GTK_POPOVER(gtk_popover_new());
    }

    Popover::operator GtkWidget*()
    {
        return GTK_WIDGET(_native);
    }

    void Popover::popup()
    {
        gtk_popover_popup(_native);
    }

    void Popover::popdown()
    {
        gtk_popover_popdown(_native);
    }

    void Popover::present()
    {
        gtk_popover_present(_native);
    }

    void Popover::set_child(Widget* widget)
    {
        gtk_popover_set_child(_native, widget->operator GtkWidget*());
    }

    void Popover::set_relative_position(GtkPositionType position)
    {
        gtk_popover_set_position(_native, position);
    }

    void Popover::set_has_arrow(bool b)
    {
        gtk_popover_set_has_arrow(_native, b);
    }

    void Popover::set_autohide(bool b)
    {
        gtk_popover_set_autohide(_native, b);
    }
}