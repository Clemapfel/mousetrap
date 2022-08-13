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
            StackID add_child(GtkWidget*, const std::string& title);

            void set_transition_duration(Time);
            void set_transition_type(GtkStackTransitionType);

            StackID get_visible_child();
            void set_visible_child(StackID);

            void set_is_homogeneous(bool horizontally, bool vertically);
            void set_interpolate(bool);

        private:
            size_t _current_id = 0;
            std::map<StackID, GtkWidget*> _id_to_child;
    };

    class StackSideBar : public Widget
    {
        public:
            StackSideBar(Stack*);
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
            GtkStackSidebar* _native;
    };
}

#include <src/stack.inl>

namespace mousetrap
{
    StackSideBar::StackSideBar(Stack* stack)
    {
        _native = GTK_STACK_SIDEBAR(gtk_stack_sidebar_new());
        gtk_stack_sidebar_set_stack(_native, stack);
    }
}