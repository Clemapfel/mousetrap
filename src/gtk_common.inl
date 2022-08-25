// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <include/gl_common.hpp>
#include <iostream>

bool gtk_initialize_opengl(GtkWindow* window)
{
    bool failed = false;
    auto *gdk_window = gtk_widget_get_display(GTK_WIDGET(window));

    GError *error_maybe = nullptr;
    auto *context = gdk_display_create_gl_context(gdk_window, &error_maybe);
    if (error_maybe != nullptr)
    {
        failed = true;
        std::cerr << "[ERROR] In gdk_window_create_gl_context: " << error_maybe->message << std::endl;
    }

    gdk_gl_context_set_required_version(context, 3, 2);
    gdk_gl_context_realize(context, &error_maybe);

    if (error_maybe != nullptr)
    {
        failed = true;
        std::cerr << "[ERROR] In gdk_gl_context_realize: " << error_maybe->message << std::endl;
    }

    gdk_gl_context_make_current(context);

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK)
    {
        failed = true;
        std::cerr << "[ERROR] In glewInit: " << glewGetErrorString(glewError) << std::endl;
    }

    GL_INITIALIZED = not failed;
    return not failed;
}

/*
void gtk_widget_get_size(GtkWidget* widget, int* w, int* h)
{
    GtkAllocation* allocation = new GtkAllocation();
    gtk_widget_get_allocation(widget, allocation);

    if (w != nullptr)
        *w = allocation->width;

    if (h != nullptr)
        *h = allocation->height;
}
*/