// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <include/render_area.hpp>

namespace mousetrap
{
    RenderArea::RenderArea()
        : WidgetImplementation<GtkGLArea>(GTK_GL_AREA(gtk_gl_area_new())),
          CTOR_SIGNAL(RenderArea, render),
          CTOR_SIGNAL(RenderArea, resize)
    {
        gtk_gl_area_set_auto_render(get_native(), TRUE);
        gtk_widget_set_size_request(GTK_WIDGET(get_native()), 1, 1);

        connect_signal_render(on_render);
        connect_signal_resize(on_resize);
    }

    void RenderArea::add_render_task(Shape* shape, Shader* shader, GLTransform* transform, BlendMode blend_mode)
    {
        if (shape == nullptr)
            return;

        _render_tasks.emplace_back(shape, shader, transform, blend_mode);
    }

    void RenderArea::add_render_task(RenderTask task)
    {
        _render_tasks.push_back(task);
    }

    void RenderArea::clear_render_tasks()
    {
        _render_tasks.clear();
    }

    void RenderArea::on_resize(RenderArea* area, gint width, gint height)
    {
        area->make_current();
    }

    gboolean RenderArea::on_render(RenderArea* area, GdkGLContext* context)
    {
        area->make_current();

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_BLEND);
        set_current_blend_mode(BlendMode::NORMAL);

        for (auto& task : area->_render_tasks)
            task.render();

        glFlush();
        return FALSE;
    }

    void RenderArea::queue_render()
    {
        gtk_gl_area_queue_render(get_native());
        gtk_widget_queue_draw(GTK_WIDGET(get_native()));
    }

    void RenderArea::make_current()
    {
        gtk_gl_area_make_current(get_native());
    }
}