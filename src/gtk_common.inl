// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <include/gl_common.hpp>
#include <iostream>

inline bool gtk_initialize_opengl(GtkWindow* window)
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

    glewExperimental = GL_FALSE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_NO_ERROR)
    {
        std::cerr << "[WARNING] In glewInit: Unable to initialize glew " << "(" << glewError << ") ";

        if (glewError == GLEW_ERROR_NO_GL_VERSION)
            std::cerr << "Missing GL version" << std::endl;
        else if (glewError == GLEW_ERROR_GL_VERSION_10_ONLY)
            std::cerr << "Need at least OpenGL 1.1" << std::endl;
        else if (glewError == GLEW_ERROR_GLX_VERSION_11_ONLY)
            std::cerr << "Need at least GLX 1.2" << std::endl;
        else if (glewError == GLEW_ERROR_NO_GLX_DISPLAY)
            std::cerr << "Need GLX Display for GLX support. Are you on Wayland?" << std::endl;
    }

    GL_INITIALIZED = true;
    return not failed;
}


