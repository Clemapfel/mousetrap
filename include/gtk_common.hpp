// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

extern "C"
{
    ///
    bool gtk_initialize_opengl(GtkWindow* window);

    ///
    void gtk_widget_get_size(GtkWidget*, int*, int*);
}


#include <src/gtk_common.inl>
