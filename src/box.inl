// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <vector>

namespace mousetrap
{
    Box::Box(GtkOrientation orientation, float spacing)
    {
        _box = GTK_BOX(gtk_box_new(orientation, spacing));
    }

    Box::~Box()
    {}

    Box HBox(float spacing)
    {
        return Box(GTK_ORIENTATION_HORIZONTAL, spacing);
    }

    Box VBox(float spacing)
    {
        return Box(GTK_ORIENTATION_VERTICAL, spacing);
    }

    GtkWidget* Box::get_native()
    {
        return GTK_WIDGET(_box);
    }

    void Box::add(GtkWidget* widget)
    {
        gtk_box_append(_box, widget);
    }

    void Box::remove(GtkWidget* widget)
    {
        gtk_box_remove(_box, widget);
    }
}