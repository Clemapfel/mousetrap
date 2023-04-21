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

    void Expander::set_child(const Widget& widget)
    {
        _child = &widget;
        WARN_IF_SELF_INSERTION(Expander::set_child, this, _child);

        gtk_expander_set_child(get_native(), widget);
    }

    void Expander::remove_child()
    {
        _child = nullptr;
        gtk_expander_set_child(get_native(), nullptr);
    }

    Widget* Expander::get_child() const
    {
        return const_cast<Widget*>(_child);
    }

    bool Expander::get_expanded()
    {
        return gtk_expander_get_expanded(get_native());
    }

    void Expander::set_expanded(bool b)
    {
        gtk_expander_set_expanded(get_native(), b);
    }

    void Expander::set_label_widget(const Widget& widget)
    {
        _label_widget = &widget;
        WARN_IF_SELF_INSERTION(Expander::set_label_widget, this, _label_widget);

        gtk_expander_set_label_widget(get_native(), widget.operator NativeWidget());
    }

    Widget* Expander::get_label_widget() const
    {
        return const_cast<Widget*>(_label_widget);
    }
}