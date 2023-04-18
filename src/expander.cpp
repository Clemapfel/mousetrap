//
// Copyright 2022 Clemens Cords
// Created on 9/18/22 by clem (mail@clemens-cords.com)
//

#include <include/expander.hpp>
#include <include/log.hpp>

namespace mousetrap
{
    Expander::Expander()
        : WidgetImplementation<GtkExpander>(GTK_EXPANDER(gtk_expander_new(nullptr))),
          CTOR_SIGNAL(Expander, activate)
    {}

    void Expander::set_child(Widget* widget)
    {
        WARN_IF_SELF_INSERTION(Expander::set_child, this, widget);

        _child = widget;
        gtk_expander_set_child(get_native(), _child == nullptr ? nullptr : _child->operator GtkWidget*());
    }

    Widget* Expander::get_child() const
    {
        return _child;
    }

    bool Expander::get_expanded()
    {
        return gtk_expander_get_expanded(get_native());
    }

    void Expander::set_expanded(bool b)
    {
        gtk_expander_set_expanded(get_native(), b);
    }

    void Expander::set_label_widget(Widget* widget)
    {
        WARN_IF_SELF_INSERTION(Expander::set_label_widget, this, widget);

        _label_widget = widget;
        gtk_expander_set_label_widget(get_native(), _label_widget == nullptr ? nullptr : _label_widget->operator GtkWidget*());
    }

    Widget* Expander::get_label_widget() const
    {
        return _label_widget;
    }
}