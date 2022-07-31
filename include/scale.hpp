// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>
#include <include/widget.hpp>
#include <include/range.hpp>

namespace rat
{
    class Scale : public Widget, public Range
    {
        public:
            Scale(float min, float max, float step, GtkOrientation orientation = GTK_ORIENTATION_HORIZONTAL);
            ~Scale();

            GtkWidget* get_native() override;

        private:
            GtkScale* _native;
    };
}

// #####

namespace rat
{
    Scale::Scale(float min, float max, float step, GtkOrientation orientation)
    {
        _native = GTK_SCALE(gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, min, max, step));
        gtk_scale_set_draw_value(_native, false);
    }

    GtkWidget* Scale::get_native()
    {
        return GTK_WIDGET(_native);
    }

    Scale::~Scale()
    {
        gtk_widget_destroy(GTK_WIDGET(_native));
    }
}