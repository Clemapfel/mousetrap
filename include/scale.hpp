// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>
#include <include/widget.hpp>
#include <include/range.hpp>

namespace mousetrap
{
    class Scale : public Range
    {
        public:
            Scale(float min, float max, float step, GtkOrientation orientation = GTK_ORIENTATION_HORIZONTAL);
            virtual ~Scale();

            GtkWidget* get_native() override;

            void set_draw_value(bool);
            void add_mark(float at, GtkPositionType pos, const std::string& label = "");
            void clear_marks();

        private:
            GtkScale* _native;
    };
}

#include <src/scale.inl>