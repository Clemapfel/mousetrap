//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/19/23
//

#pragma once

#include <include/widget.hpp>
#include <include/shape.hpp>
#include <include/render_task.hpp>

#ifdef DOXYGEN
    #include "../docs/doxygen.inl"
#endif

namespace mousetrap
{
    /// @brief area that allows OpenGL primitives to be rendered
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
            std::vector<RenderTask> _render_tasks;
    };
}