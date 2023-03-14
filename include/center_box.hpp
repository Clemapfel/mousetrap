// 
// Copyright 2022 Clemens Cords
// Created on 9/30/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class CenterBox : public WidgetImplementation<GtkCenterBox>
    {
        public:
            CenterBox(GtkOrientation);

            void set_start_widget(Widget*);
            void set_center_widget(Widget*);
            void set_end_widget(Widget*);

            void set_baseline_position(GtkBaselinePosition);
    };
}
