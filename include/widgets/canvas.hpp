// 
// Copyright 2022 Clemens Cords
// Created on 8/9/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/frame.hpp>

namespace mousetrap
{
    class Canvas : public Widget
    {
        public:
            Canvas(size_t width, size_t height);

            GtkWidget* get_native() override {
                return GTK_WIDGET(_frame);
            }

        private:
            static void on_render(GtkGLArea*, Canvas* instance);
            static void on_resize(GtkGLArea*, int, int, Canvas* instance);
            static void on_realize(GtkGLArea*, Canvas* instance);

            Frame* _frame;
            GtkGLArea* _gl_area;
    };
}

// ###

namespace mousetrap
{
    Canvas::Canvas(size_t width, size_t height)
    {
        _gl_area = GTK_GL_AREA(gtk_gl_area_new());
        gtk_gl_area_set_has_alpha(_gl_area, TRUE);
        gtk_widget_set_size_request(GTK_WIDGET(_gl_area), width, height);

        g_signal_connect(_gl_area, "resize", G_CALLBACK(on_resize), this);
        g_signal_connect(_gl_area, "render", G_CALLBACK(on_render), this);
        g_signal_connect(_gl_area, "realize", G_CALLBACK(on_realize), this);
    }

}