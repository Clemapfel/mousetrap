// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/container.hpp>

namespace rat
{
    class Box : public Widget, public Container
    {
        public:
            Box(GtkOrientation, float spacing = 0);
            ~Box();

            GtkWidget* get_native();
            using Container::add;

        private:
            GtkBox* _box;
    };

    Box HBox(float spacing);
    Box VBox(float spacing);
}

// ###

namespace rat
{
    Box::Box(GtkOrientation orientation, float spacing)
    {
        _box = GTK_BOX(gtk_box_new(orientation, spacing));
    }

    Box::~Box()
    {
        gtk_widget_destroy(GTK_WIDGET(_box));
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