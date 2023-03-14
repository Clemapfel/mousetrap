// 
// Copyright 2022 Clemens Cords
// Created on 9/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class Expander : public WidgetImplementation<GtkExpander>, public HasActivateSignal<Expander>
    {
        public:
            Expander(const std::string&);

            void set_child(Widget*);

            void set_label_widget(Widget*);
            void set_resize_toplevel(bool);

            bool get_expanded();
            void set_expanded(bool);
    };
}

#include <src/expander.inl>
