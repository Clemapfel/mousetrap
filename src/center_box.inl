// 
// Copyright 2022 Clemens Cords
// Created on 10/26/22 by clem (mail@clemens-cords.com)
//

#include <include/center_box.hpp>

namespace mousetrap
{
    CenterBox::CenterBox(GtkOrientation orientation)
            : WidgetImplementation<GtkCenterBox>(GTK_CENTER_BOX(gtk_center_box_new()))
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(get_native()), orientation);
    }

    void CenterBox::set_start_widget(Widget* widget)
    {
        gtk_center_box_set_start_widget(get_native(), widget->operator GtkWidget*());
    }

    void CenterBox::set_end_widget(Widget* widget)
    {
        gtk_center_box_set_end_widget(get_native(), widget->operator GtkWidget*());
    }

    void CenterBox::set_center_widget(Widget* widget)
    {
        gtk_center_box_set_center_widget(get_native(), widget->operator GtkWidget*());
    }

    void CenterBox::set_baseline_position(GtkBaselinePosition position)
    {
        gtk_center_box_set_baseline_position(get_native(), position);
    }
}