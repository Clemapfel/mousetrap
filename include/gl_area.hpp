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
    class GLArea : public WidgetImplementation<GtkGLArea>
    {
        public:
            GLArea();

            void add_render_task(Shape*, Shader* = nullptr, GLTransform* = nullptr);
            void add_render_task(RenderTask);
            void clear_render_tasks();

            void queue_render();
            void make_current();

            template<typename T>
            using on_render_function_t = bool(GLArea*, GdkGLContext*, T);

            template<typename Function_t, typename T>
            void connect_signal_render(Function_t, T);

            template<typename T>
            using on_resize_function_t = void(GLArea*, int, int, T);

            template<typename Function_t, typename T>
            void connect_signal_resize(Function_t, T);

        private:
            static gboolean on_render_wrapper(GtkGLArea*, GdkGLContext*, GLArea* instance);
            std::function<on_render_function_t<void*>> _on_render_f;
            void* _on_render_data;

            static void on_resize_wrapper(GtkGLArea*, int, int, GLArea* instance);
            std::function<on_resize_function_t<void*>> _on_resize_f;
            void* _on_resize_data;

            static void on_resize(GLArea* area, gint width, gint height, void*);
            static gboolean on_render(GLArea*, GdkGLContext*, void*);

            std::vector<RenderTask> _render_tasks;
    };
}

#include <src/gl_area.inl>
