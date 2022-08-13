// 
// Copyright 2022 Clemens Cords
// Created on 8/11/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/adjustment.hpp>

namespace mousetrap
{
    class Scrollbar : public Widget
    {
        public:
            Scrollbar(Adjustment, GtkOrientation = GTK_ORIENTATION_HORIZONTAL);
            Scrollbar(GtkOrientation = GTK_ORIENTATION_HORIZONTAL);

            GtkWidget* get_native() override;
            Adjustment& get_adjustment();

        private:
            GtkScrollbar* _native;
            Adjustment _adjustment;
    };
}

#include <src/scrollbar.inl>