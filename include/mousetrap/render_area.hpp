//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/19/23
//

#pragma once

#include <mousetrap/gl_common.hpp>
#if MOUSETRAP_ENABLE_OPENGL_COMPONENT

#include <mousetrap/widget.hpp>
#include <mousetrap/shape.hpp>
#include <mousetrap/render_task.hpp>

#ifdef DOXYGEN
    #include "../../docs/doxygen.hpp"
#endif

namespace mousetrap
{
    namespace detail
    {
        /// @brief global OpenGL context
        inline GdkGLContext* GL_CONTEXT = nullptr;

        /// @brief initialize the global OpenGL context
        GdkGLContext* initialize_opengl();

        /// @brief free the global OpenGL context
        void shutdown_opengl();

        /// @brief get whether gl context is disabled
        bool is_opengl_disabled();
    }

    #ifndef DOXYGEN
    class RenderArea;
    class MultisampledRenderTexture;
    namespace detail
    {
        struct _RenderAreaInternal
        {
            GObject parent;
            GtkGLArea* native;
            std::vector<detail::RenderTaskInternal*>* tasks;

            bool apply_msaa;
            MultisampledRenderTexture* render_texture;
            Shape* render_texture_shape;
            RenderTask* render_texture_shape_task;
            Shader* render_texture_shader;
        };
        using RenderAreaInternal = _RenderAreaInternal;
        DEFINE_INTERNAL_MAPPING(RenderArea);
    }
    #endif

    class RenderTask;

    /// @brief quality of MSAA aliasing applied to render area
    enum class AntiAliasingQuality
    {
        /// @brief no anti aliasing
        OFF = 0,

        /// @brief MSAA with 2 samples per fragment
        MINIMAL = 2,

        /// @brief MSAA with 4 samples per fragment
        GOOD = 4,

        /// @brief MSAA with 8 samples per fragment
        BETTER = 8,

        /// @brief MSAA with 16 samples per fragment
        BEST = 16,
    };

    /// @brief area that allows OpenGL primitives to be rendered
    /// \signals
    /// \signal_render{RenderArea}
    /// \signal_resize{RenderArea}
    /// \widget_signals{RenderArea}
    class RenderArea :
        public detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(RenderArea, render),
        HAS_SIGNAL(RenderArea, resize),
        HAS_SIGNAL(RenderArea, realize),
        HAS_SIGNAL(RenderArea, unrealize),
        HAS_SIGNAL(RenderArea, destroy),
        HAS_SIGNAL(RenderArea, hide),
        HAS_SIGNAL(RenderArea, show),
        HAS_SIGNAL(RenderArea, map),
        HAS_SIGNAL(RenderArea, unmap)
    {
        public:
            /// @brief constrcut
            /// @param msaa_samples
            RenderArea(AntiAliasingQuality msaa_samples = AntiAliasingQuality::OFF);

            /// @brief destructor
            ~RenderArea();

            /// @brief construct from internal
            RenderArea(detail::RenderAreaInternal*);

            /// @brief expose internal
            NativeObject get_internal() const;

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

            /// @brief flush the current state of the bound frame buffer, exactly equivalent to `glFlush()`. Call  RenderArea::queue_render` in order for the screen to update
            static void flush();

            /// @brief clear the currently bound framebuffer, equivalent to `glClearColor(0, 0, 0, 0); glClear(GL_COLOR_BUFFER_BIT);`
            static void clear();

        private:
            static void on_realize(GtkWidget* area, detail::RenderAreaInternal*);
            static void on_resize(GtkGLArea* area, gint width, gint height, detail::RenderAreaInternal*);
            static gboolean on_render(GtkGLArea*, GdkGLContext*, detail::RenderAreaInternal*);
            static GdkGLContext* on_create_context(GtkGLArea*, GdkGLContext*, detail::RenderAreaInternal*);

            detail::RenderAreaInternal* _internal = nullptr;
    };
}

#endif // MOUSETRAP_ENABLE_OPENGL_COMPONENT