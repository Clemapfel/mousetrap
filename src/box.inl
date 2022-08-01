// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

namespace mousetrap
{
    Box::Box(GtkOrientation orientation, float spacing)
    {
        _box = GTK_BOX(gtk_box_new(orientation, spacing));
    }

    Box::~Box()
    {
        //gtk_widget_destroy(GTK_WIDGET(_box));
    }

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
}