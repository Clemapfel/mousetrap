// 
// Copyright 2022 Clemens Cords
// Created on 8/13/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    StackSidebar::StackSidebar(Stack* stack)
    {
        _native = GTK_STACK_SIDEBAR(gtk_stack_sidebar_new());
        gtk_stack_sidebar_set_stack(_native, GTK_STACK(stack->get_native()));
    }

    StackSidebar::operator GtkWidget*()
    {
        return GTK_WIDGET(_native);
    }

    StackSwitcher::StackSwitcher(Stack* stack)
    {
        _native = GTK_STACK_SWITCHER(gtk_stack_switcher_new());
        gtk_stack_switcher_set_stack(_native, GTK_STACK(stack->get_native()));
    }

    StackSwitcher::operator GtkWidget*()
    {
        return GTK_WIDGET(_native);
    }

    Stack::Stack()
    {
        _native = GTK_STACK(gtk_stack_new());
    }

    Stack::operator GtkWidget*()
    {
        return GTK_WIDGET(_native);
    }

    StackID Stack::add_child(Widget* widget, const std::string& title)
    {
        auto id = _current_id++;
        gtk_stack_add_titled(_native, widget->get_native(), std::to_string(id).c_str(), title.c_str());
        _id_to_widget.insert({id, widget});
        return id;
    }

    void Stack::remove_child(StackID id)
    {
        gtk_container_remove(GTK_CONTAINER(_native), _id_to_widget.at(id)->get_native());
        _id_to_widget.erase(id);
    }

    StackID Stack::get_visible_child()
    {
        return std::stoi(gtk_stack_get_visible_child_name(_native));
    }

    void Stack::set_visible_child(StackID id)
    {
        gtk_stack_set_visible_child_name(_native, std::to_string(id).c_str());
    }

    void Stack::set_transition_duration(Time time)
    {
        gtk_stack_set_transition_duration(_native, time.as_milliseconds());
    }

    void Stack::set_transition_type(GtkStackTransitionType type)
    {
        gtk_stack_set_transition_type(_native, type);
    }

    void Stack::set_is_homogeneous(bool horizontally, bool vertically)
    {
        gtk_stack_set_hhomogeneous(_native, horizontally);
        gtk_stack_set_vhomogeneous(_native, vertically);
    }

    void Stack::set_interpolate_size(bool b)
    {
        gtk_stack_set_interpolate_size(_native, b);
    }
}

