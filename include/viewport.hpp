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
            Viewport(Adjustment& h_adjustment, Adjustment& v_adjustment);
            void set_child(Widget* widget);

    };
}

namespace mousetrap
{
    Viewport::Viewport(Adjustment& h_adjustment, Adjustment& v_adjustment)
        : WidgetImplementation<GtkViewport>(GTK_VIEWPORT(gtk_viewport_new(
                  h_adjustment.operator GtkAdjustment*(),
                  v_adjustment.operator GtkAdjustment*()))
          )
    {}

    void Viewport::set_child(Widget* widget)
    {
        gtk_viewport_set_child(get_native(), widget->operator GtkWidget*());
    }
}