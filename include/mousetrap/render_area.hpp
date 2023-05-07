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
        GdkGLContext* initialize_opengl();
    }

    #ifndef DOXYGEN
    namespace detail
    {
        struct _RenderAreaInternal
        {
            GObject parent;
            std::vector<RenderTask>* tasks;
        };
        using RenderAreaInternal = _RenderAreaInternal;
    }
    #endif

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
            void add_render_task(const Shape&, Shader* = nullptr, GLTransform* = nullptr, BlendMode = BlendMode::NORMAL);

            /// @brief add render task
            /// @param task allocated render task, this object will take ownership of the task
            void add_render_task(RenderTask task);

            /// @brief unregister all render tasks
            void clear_render_tasks();

            /// @brief notify the area that a re-render should be done as soon as possible
            void queue_render();

            /// @brief make the areas OpenGL context the currently active context, this is usually not necessary to call
            void make_current();

            /// @brief convert gl coordinates to absolut widget-space coordinates
            /// @param gl_space_coordinate vector, in GL coordinates ([-1, 1], [-1, 1]) with origin at (0, 0)
            /// @return vector, in Widget-space coordinates([0, width], [0, height]) with origin at (0.5 * width, 0.5 * height)
            Vector2f from_gl_coordinates(Vector2f gl_space_coordinate);

            /// @brief convert widget-space coordinates to gl
            /// @param widget_space_coordinate Widget-space coordinates([0, width], [0, height]) with origin at (0.5 * width, 0.5 * height)
            /// @return vector, in GL coordinates ([-1, 1], [-1, 1]) with origin at (0, 0)
            Vector2f to_gl_coordinates(Vector2f widget_space_coordinate);

        private:
            static void on_realize(Widget* area);
            static void on_resize(RenderArea* area, gint width, gint height);
            static gboolean on_render(RenderArea*, GdkGLContext*);
            static GdkGLContext* on_create_context(GtkGLArea*, GdkGLContext*);
            detail::RenderAreaInternal* _internal = nullptr;
    };
}