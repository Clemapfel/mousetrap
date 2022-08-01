// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    GtkWidget* GLArea::get_native()
    {
        return GTK_WIDGET(_native);
    }

    GLArea::GLArea()
    {
        _native = GTK_GL_AREA(gtk_gl_area_new());

        connect_signal("realize", on_realize_wrapper, this);
        connect_signal("render", on_render_wrapper, this);
        connect_signal("unrealize", on_shutdown_wrapper, this);
        connect_signal("resize", on_resize_wrapper, this);
    }

    GLArea::~GLArea()
    {
        //gtk_widget_destroy(GTK_WIDGET(_native));
    }

    void GLArea::add_render_task(Shape* shape, Shader* shader, Transform* transform)
    {
        _render_tasks.emplace_back(shape, shader, transform);
    }

    void GLArea::on_realize_wrapper(void* area, void* instance)
    {
        ((GLArea*) instance)->on_realize(GTK_GL_AREA(area));
    }

    gboolean GLArea::on_render_wrapper(void* area, void* context, void* instance)
    {
        return ((GLArea*) instance)->on_render(GTK_GL_AREA(area), GDK_GL_CONTEXT(context));
    }

    void GLArea::on_shutdown_wrapper(void* area, void* instance)
    {
        ((GLArea*) instance)->on_shutdown(GTK_GL_AREA(area));
    }

    void GLArea::on_resize_wrapper(GtkGLArea* area, gint width, gint height, void* instance)
    {
        ((GLArea*) instance)->on_resize(area, width, height);
    }

    void GLArea::on_realize(GtkGLArea* area)
    {
        // noop
    }

    void GLArea::on_resize(GtkGLArea* area, gint width, gint height)
    {
        gtk_gl_area_queue_render(area);
    }

    void GLArea::on_shutdown(GtkGLArea*)
    {
        // noop
    }

    gboolean GLArea::on_render(GtkGLArea* area, GdkGLContext* context)
    {
        gtk_gl_area_make_current(area);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        for (auto& task : _render_tasks)
            task.render();

        glFlush();
        return FALSE;
    }
}