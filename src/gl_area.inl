// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    GLArea::GLArea()
        : WidgetImplementation<GtkGLArea>(GTK_GL_AREA(gtk_gl_area_new()))
    {
        gtk_gl_area_set_auto_render(get_native(), TRUE);
        gtk_widget_set_size_request(GTK_WIDGET(get_native()), 1, 1);

        connect_signal_render(on_render, this);
        connect_signal_resize(on_resize, this);
    }

    void GLArea::add_render_task(Shape* shape, Shader* shader, GLTransform* transform)
    {
        if (shape == nullptr)
            return;

        _render_tasks.emplace_back(shape, shader, transform);
    }

    void GLArea::add_render_task(RenderTask task)
    {
        _render_tasks.push_back(task);
    }

    void GLArea::clear_render_tasks()
    {
        _render_tasks.clear();
    }

    void GLArea::on_resize(GLArea* area, gint width, gint height, void*)
    {
        area->make_current();
    }

    gboolean GLArea::on_render(GLArea* area, GdkGLContext* context, void*)
    {
        area->make_current();

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (auto& task : area->_render_tasks)
            task.render();

        glFlush();
        return FALSE;
    }

    void GLArea::queue_render()
    {
        gtk_gl_area_queue_render(get_native());
        gtk_widget_queue_draw(GTK_WIDGET(get_native()));
    }

    void GLArea::make_current()
    {
        gtk_gl_area_make_current(get_native());
    }

    template<typename Function_t, typename T>
    void GLArea::connect_signal_render(Function_t f, T data)
    {
        auto temp =  std::function<on_render_function_t<T>>(f);
        _on_render_f = std::function<on_render_function_t<void*>>(*((std::function<on_render_function_t<void*>>*) &temp));
        _on_render_data = data;

        connect_signal("render", on_render_wrapper, this);
    }

    template<typename Function_t, typename T>
    void GLArea::connect_signal_resize(Function_t f, T data)
    {
        auto temp =  std::function<on_resize_function_t<T>>(f);
        _on_resize_f = std::function<on_resize_function_t<void*>>(*((std::function<on_resize_function_t<void*>>*) &temp));
        _on_resize_data = data;

        connect_signal("render", on_render_wrapper, this);
    }

    gboolean GLArea::on_render_wrapper(GtkGLArea* area, GdkGLContext* context, GLArea* instance)
    {
        if (instance->_on_render_f == nullptr)
            return FALSE;

        return (instance->_on_render_f)(instance, context, instance->_on_render_data);
    }

    void GLArea::on_resize_wrapper(GtkGLArea* area, int w, int h, GLArea* instance)
    {
        if (instance->_on_render_f == nullptr)
            return;

        (instance->_on_resize_f)(instance, w, h, instance->_on_render_data);
    }
}