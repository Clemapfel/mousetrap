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
    class RenderArea;
    namespace detail
    {
        struct _RenderAreaInternal
        {
            GObject parent;
            GtkGLArea* native;
            std::vector<detail::RenderTaskInternal*>* tasks;
        };
        using RenderAreaInternal = _RenderAreaInternal;

        template<>
        struct InternalMapping<RenderArea>
        {
            using internal = RenderAreaInternal;
        };
    }
    #endif

    class RenderTask;

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

            /// @brief destructor
            ~RenderArea();

            /// @brief constructo from internal
            RenderArea(detail::RenderAreaInternal*);

            /// @brief add render task
            /// @param task allocated render task, this object will take ownership of the task
            void add_render_task(RenderTask task);

            /// @brief unregister all render tasks
            void clear_render_tasks();

            /// @brief trigger the `render` function of all registered render tasks
            void render_render_tasks();

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

            /// @brief expose internal \for_internal_use_only
            GObject* get_internal() const;

        private:
            static void on_realize(GtkWidget* area, detail::RenderAreaInternal*);
            static void on_resize(GtkGLArea* area, gint width, gint height, detail::RenderAreaInternal*);
            static gboolean on_render(GtkGLArea*, GdkGLContext*, detail::RenderAreaInternal*);
            static GdkGLContext* on_create_context(GtkGLArea*, GdkGLContext*, detail::RenderAreaInternal*);

            detail::RenderAreaInternal* _internal = nullptr;
    };
}