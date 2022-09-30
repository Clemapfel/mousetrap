// 
// Copyright 2022 Clemens Cords
// Created on 9/28/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/adjustment.hpp>

namespace mousetrap
{
    class Viewport : public WidgetImplementation<GtkViewport>
    {
        public:
            Viewport();
            Viewport(Adjustment h_adjustment, Adjustment v_adjustment);

            void set_child(Widget* widget);

        private:
            Adjustment _h_adjustment;
            Adjustment _v_adjustment;
    };
}

namespace mousetrap
{
    Viewport::Viewport(Adjustment h_adjustment, Adjustment v_adjustment)
        : WidgetImplementation<GtkViewport>(GTK_VIEWPORT(gtk_viewport_new(
                  _h_adjustment.operator GtkAdjustment*(),
                  _v_adjustment.operator GtkAdjustment*()))
          ),
          _h_adjustment(h_adjustment),
          _v_adjustment(v_adjustment)
    {}

    void Viewport::set_child(Widget* widget)
    {
        gtk_viewport_set_child(get_native(), widget->operator GtkWidget*());
    }
}