// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/container.hpp>

namespace rat
{
    class Overlay : public Container
    {
        public:
            Overlay();
            ~Overlay();

            GtkWidget* get_native();

            template<typename T>
            void set_under(T);

            template<typename T>
            void set_over(T);

        private:
            GtkOverlay* _overlay;
    };
}

// ###

namespace rat
{
    Overlay::Overlay()
    {
        _overlay = GTK_OVERLAY(gtk_overlay_new());
    }

    Overlay::~Overlay()
    {
        gtk_widget_destroy(GTK_WIDGET(_overlay));
    }

    template<typename T>
    void Overlay::set_under(T in)
    {
        gtk_container_add(GTK_CONTAINER(_overlay), (GtkWidget*) in);
    }

    template<typename T>
    void Overlay::set_over(T in)
    {
        gtk_overlay_add_overlay(_overlay, (GtkWidget*) in);
    }
}