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
    class GLArea : public Widget
    {
        public:
            GLArea();
            virtual ~GLArea();

            GtkWidget* get_native() override;

            void add_render_task(Shape*, Shader* = nullptr, GLTransform* = nullptr);
            void add_render_task(RenderTask);

            void queue_render();

        private:
            void on_resize(GtkGLArea* area, gint width, gint height);
            gboolean on_render(GtkGLArea*, GdkGLContext*);

            static gboolean on_render_wrapper(void* area, void* context, void* instance);
            static void on_resize_wrapper(GtkGLArea* area, gint width, gint height, void* instance);

            GtkGLArea* _native;
            std::vector<RenderTask> _render_tasks;
    };
}

#include <src/gl_area.inl>
