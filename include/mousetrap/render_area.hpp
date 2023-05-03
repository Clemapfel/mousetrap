//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/19/23
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/shape.hpp>
#include <mousetrap/render_task.hpp>

#ifdef DOXYGEN
    #include "../../docs/doxygen.inl"
#endif

namespace mousetrap
{
    namespace detail
    {
        /// @brief whether OpenGL has been initialized yet
        inline bool GL_INITIALIZED = false;

        /// @brief global OpenGL context
        inline GdkGLContext* GL_CONTEXT = nullptr;

        /// @brief initialize the global OpenGL context
        static GdkGLContext* initialize_opengl()
        {
            if (not detail::GL_INITIALIZED)
            {
                bool failed = false;
                GError *error_maybe = nullptr;
                detail::GL_CONTEXT = gdk_display_create_gl_context(gdk_display_get_default(), &error_maybe);
                auto* context = detail::GL_CONTEXT;
                g_object_ref_sink(context);

                if (error_maybe != nullptr)
                {
                    failed = true;
                    log::critical("In gdk_window_create_gl_context: " +  std::string(error_maybe->message));
                    g_error_free(error_maybe);
                }

                gdk_gl_context_set_required_version(context, 3, 2);
                gdk_gl_context_realize(context, &error_maybe);

                if (error_maybe != nullptr)
                {
                    failed = true;
                    log::critical("In gdk_gl_context_realize: " +  std::string(error_maybe->message));
                    g_error_free(error_maybe);
                }

                gdk_gl_context_make_current(context);

                glewExperimental = GL_FALSE;
                GLenum glewError = glewInit();
                if (glewError != GLEW_NO_ERROR)
                {
                    std::stringstream str;
                    str << "In glewInit: Unable to initialize glew " << "(" << glewError << ") ";

                    if (glewError == GLEW_ERROR_NO_GL_VERSION)
                        str << "Missing GL version";
                    else if (glewError == GLEW_ERROR_GL_VERSION_10_ONLY)
                        str << "Need at least OpenGL 1.1";
                    else if (glewError == GLEW_ERROR_GLX_VERSION_11_ONLY)
                        str << "Need at least GLX 1.2";
                    else if (glewError == GLEW_ERROR_NO_GLX_DISPLAY)
                        str << "Need GLX Display for GLX support";

                    log::warning(str.str());
                }

                log::info("succesfully initialized OpenGL");
                detail::GL_INITIALIZED = true;
            }

            return GL_CONTEXT;
        }
    }

    /// @brief area that allows OpenGL primitives to be rendered
    /// \signals
    /// \signal_render{RenderArea}
    /// \signal_resize{RenderArea}
    /// \widget_signals{RenderArea}
    class RenderArea : public WidgetImplementation<GtkGLArea>,
        HAS_SIGNAL(RenderArea, render),
        HAS_SIGNAL(RenderArea, resize)
    {
        public:
            /// @brief constrcut
            RenderArea();

            /// @brief add render task, unless the <tt>render</tt> signal was overwritten, the default signal handler will render all render tasks in the order they were added
            /// @param shape shape to hand to the render task
            /// @param shader shader to hand to the render task, or nullptr to use the default shader
            /// @param transform transform to hand to the render task, or nullptr to use the identity transform
            /// @param blend_mode BlendMode to hand to the render task, or nullptr to use normal alpha blending
            void add_render_task(Shape*, Shader* = nullptr, GLTransform* = nullptr, BlendMode = BlendMode::NORMAL);

            /// @brief add render task
            /// @param already allocated render task, this object will take ownership of the task
            void add_render_task(RenderTask);

            /// @brief unregister all render tasks
            void clear_render_tasks();

            /// @brief notify the area that a re-render should be done as soon as possible
            void queue_render();

            /// @brief make the areas OpenGL context the currently active context
            void make_current();

        private:
            static void on_resize(RenderArea* area, gint width, gint height);
            static gboolean on_render(RenderArea*, GdkGLContext*);
            static GdkGLContext* on_create_context(GtkGLArea*, GdkGLContext*);
            std::vector<RenderTask> _render_tasks;
    };
}