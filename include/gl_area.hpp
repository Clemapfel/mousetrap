// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gtk_common.hpp>
#include <include/widget.hpp>
#include <include/render_task.hpp>

namespace mousetrap
{
    class GLArea : public WidgetImplementation<GtkGLArea>, public HasRenderSignal<GLArea>, public HasResizeSignal<GLArea>
    {
        public:
            GLArea();

            void add_render_task(Shape*, Shader* = nullptr, GLTransform* = nullptr);
            void add_render_task(RenderTask);
            void clear_render_tasks();

            void queue_render();
            void make_current();

        private:
            static void on_resize(GLArea* area, gint width, gint height, void*);
            static gboolean on_render(GLArea*, GdkGLContext*, void*);

            std::vector<RenderTask> _render_tasks;
    };
}
