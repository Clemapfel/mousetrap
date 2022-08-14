// 
// Copyright 2022 Clemens Cords
// Created on 8/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/container.hpp>

namespace mousetrap
{
    class FlowBox : public Container
    {
        public:
            FlowBox();

            GtkWidget* get_native();

            void set_activate_on_single_click(bool);

            void set_min_children_per_line(size_t);
            void set_max_children_per_line(size_t);

            void set_column_spacing(float);
            void set_row_spacing(float);

            void set_selection_mode(GtkSelectionMode);

        private:
            GtkFlowBox* _native;
    };
}

#include <src/flow_box.inl>