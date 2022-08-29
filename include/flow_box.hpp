// 
// Copyright 2022 Clemens Cords
// Created on 8/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class FlowBox : public Widget
    {
        public:
            FlowBox();

            operator GtkWidget*() override;

            void set_activate_on_single_click(bool);

            void set_min_children_per_line(size_t);
            void set_max_children_per_line(size_t);

            void set_column_spacing(float);
            void set_row_spacing(float);

            void set_selection_mode(GtkSelectionMode);

            void set_homogeneous(bool);

            GtkFlowBoxChild* get_child_at_index(size_t i);
            GtkFlowBoxChild* get_child_at_pos(int x, int y);

            void push_back(Widget*);
            void push_front(Widget*);
            void insert_at(Widget*, size_t);
            void remove(Widget*);

            void clear();

        private:
            GtkFlowBox* _native;
    };
}

#include <src/flow_box.inl>