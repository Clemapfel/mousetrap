// 
// Copyright 2022 Clemens Cords
// Created on 8/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/signal_component.hpp>

namespace mousetrap
{
    class FlowBox : public WidgetImplementation<GtkFlowBox>,
            public HasChildActivatedSignal<FlowBox>
    {
        public:
            FlowBox(GtkOrientation = GTK_ORIENTATION_VERTICAL);

            void set_orientation(GtkOrientation);
            void set_activate_on_single_click(bool);

            void set_min_children_per_line(size_t);
            void set_max_children_per_line(size_t);

            void set_column_spacing(float);
            void set_row_spacing(float);

            void set_selection_mode(GtkSelectionMode);
            void set_homogeneous(bool);

            void select(size_t i);
            void unselect_all();

            void push_back(Widget*);
            void push_front(Widget*);
            void insert_at(Widget*, size_t);
            void remove(Widget*);

            void clear();
    };
}

#include <src/flow_box.inl>