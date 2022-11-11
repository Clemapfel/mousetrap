// 
// Copyright 2022 Clemens Cords
// Created on 8/13/22 by clem (mail@clemens-cords.com)
//

#include <include/stack.hpp>

namespace mousetrap
{
    StackSidebar::StackSidebar(Stack* stack)
        : WidgetImplementation<GtkStackSidebar>(GTK_STACK_SIDEBAR(gtk_stack_sidebar_new()))
    {
        gtk_stack_sidebar_set_stack(get_native(), stack->operator GtkStack*());
    }


    StackSwitcher::StackSwitcher(Stack* stack)
        : WidgetImplementation<GtkStackSwitcher>(GTK_STACK_SWITCHER(gtk_stack_switcher_new()))
    {
        gtk_stack_switcher_set_stack(get_native(), GTK_STACK(stack->operator GtkStack*()));
    }

    Stack::Stack()
        : WidgetImplementation<GtkStack>(GTK_STACK(gtk_stack_new()))
    {}

    StackID Stack::add_child(Widget* widget, const std::string& title)
    {
        auto id = _current_id++;
        gtk_stack_add_titled(get_native(), widget == nullptr ? nullptr : widget->operator GtkWidget*(), std::to_string(id).c_str(), title.c_str());
        _id_to_widget.insert({id, widget});
        return id;
    }

    void Stack::remove_child(StackID id)
    {
        gtk_stack_remove(get_native(), _id_to_widget.at(id)->operator GtkWidget *());
        _id_to_widget.erase(id);
    }

    StackID Stack::get_visible_child()
    {
        return std::stoi(gtk_stack_get_visible_child_name(get_native()));
    }

    void Stack::set_visible_child(StackID id)
    {
        gtk_stack_set_visible_child_name(get_native(), std::to_string(id).c_str());
    }

    void Stack::set_transition_duration(Time time)
    {
        gtk_stack_set_transition_duration(get_native(), time.as_milliseconds());
    }

    void Stack::set_transition_type(GtkStackTransitionType type)
    {
        gtk_stack_set_transition_type(get_native(), type);
    }

    void Stack::set_is_homogeneous(bool horizontally, bool vertically)
    {
        gtk_stack_set_hhomogeneous(get_native(), horizontally);
        gtk_stack_set_vhomogeneous(get_native(), vertically);
    }

    void Stack::set_interpolate_size(bool b)
    {
        gtk_stack_set_interpolate_size(get_native(), b);
    }
}

