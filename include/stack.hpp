// 
// Copyright 2022 Clemens Cords
// Created on 8/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/container.hpp>
#include <include/time.hpp>

namespace mousetrap
{
    using StackID = size_t;

    class Stack : public Container
    {
        public:
            Stack();

            GtkWidget* get_native() override;

            StackID add_child(Widget*, const std::string& title);
            void remove_child(StackID);
            StackID get_visible_child();
            void set_visible_child(StackID);

            void set_transition_duration(Time);
            void set_transition_type(GtkStackTransitionType);

            void set_is_homogeneous(bool horizontally, bool vertically);
            void set_interpolate_size(bool);

        private:
            size_t _current_id = 0;
            std::map<StackID, Widget*> _id_to_widget;

            GtkStack* _native;
    };

    class StackSidebar : public Widget
    {
        public:
            StackSidebar(Stack*);
            GtkWidget* get_native() override;

        private:
            GtkStackSidebar* _native;
    };

    class StackSwitcher : public Widget
    {
        public:
            StackSwitcher(Stack*);
            GtkWidget* get_native() override;

        private:
            GtkStackSwitcher* _native;
    };
}

#include <src/stack.inl>

namespace mousetrap
{
    StackSidebar::StackSidebar(Stack* stack)
    {
        _native = GTK_STACK_SIDEBAR(gtk_stack_sidebar_new());
        gtk_stack_sidebar_set_stack(_native, GTK_STACK(stack->get_native()));
    }

    GtkWidget* StackSidebar::get_native()
    {
        return GTK_WIDGET(_native);
    }

    StackSwitcher::StackSwitcher(Stack* stack)
    {
        _native = GTK_STACK_SWITCHER(gtk_stack_switcher_new());
        gtk_stack_switcher_set_stack(_native, GTK_STACK(stack->get_native()));
    }

    GtkWidget* StackSwitcher::get_native()
    {
        return GTK_WIDGET(_native);
    }

    Stack::Stack()
    {
        _native = GTK_STACK(gtk_stack_new());
    }

    GtkWidget* Stack::get_native()
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