// 
// Copyright 2022 Clemens Cords
// Created on 8/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/time.hpp>

namespace mousetrap
{
    using StackID = size_t;

    class Stack : public WidgetImplementation<GtkStack>
    {
        public:
            Stack();

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
    };

    class StackSidebar : public WidgetImplementation<GtkStackSidebar>
    {
        public:
            StackSidebar(Stack*);
    };

    class StackSwitcher : public WidgetImplementation<GtkStackSwitcher>
    {
        public:
            StackSwitcher(Stack*);
    };
}

