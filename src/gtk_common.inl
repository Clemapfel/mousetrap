// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <include/gl_common.hpp>
#include <iostream>

bool mousetrap::gtk_initialize_opengl(GtkWindow* window)
{
    bool failed = false;
    auto *gdk_window = gtk_widget_get_window(GTK_WIDGET(window));

    GError *error_maybe = nullptr;
    auto *context = gdk_window_create_gl_context(gdk_window, &error_maybe);
    if (error_maybe != nullptr)
    {
        failed = true;
        std::cerr << "[ERROR] In gdk_window_create_gl_context: " << error_maybe->message << std::endl;
    }

    gdk_gl_context_set_required_version(context, 3, 2);
    gdk_gl_context_set_use_es(context, TRUE);

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
