// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/render_area.hpp>
#include <mousetrap/render_task.hpp>

namespace mousetrap
{
    namespace detail
    {
        GdkGLContext* initialize_opengl()
        {
            if (not detail::GL_INITIALIZED)
            {
                bool failed = false;
                GError *error_maybe = nullptr;
                detail::GL_CONTEXT = gdk_display_create_gl_context(gdk_display_get_default(), &error_maybe);
                auto* context = detail::GL_CONTEXT;
                g_object_ref_sink(context);

                if (error_maybe != nullptr)
                {
                    failed = true;
                    log::critical("In gdk_window_create_gl_context: " +  std::string(error_maybe->message));
                    g_error_free(error_maybe);
                }

                gdk_gl_context_set_required_version(context, 3, 3);
                gdk_gl_context_realize(context, &error_maybe);

                if (error_maybe != nullptr)
                {
                    failed = true;
                    log::critical("In gdk_gl_context_realize: " +  std::string(error_maybe->message));
                    g_error_free(error_maybe);
                }

                gdk_gl_context_make_current(context);

                glewExperimental = GL_FALSE;
                GLenum glewError = glewInit();
                if (glewError != GLEW_NO_ERROR)
                {
                    std::stringstream str;
                    str << "In glewInit: Unable to initialize glew " << "(" << glewError << ") ";

                    if (glewError == GLEW_ERROR_NO_GL_VERSION)
                        str << "Missing GL version";
                    else if (glewError == GLEW_ERROR_GL_VERSION_10_ONLY)
                        str << "Need at least OpenGL 1.1";
                    else if (glewError == GLEW_ERROR_GLX_VERSION_11_ONLY)
                        str << "Need at least GLX 1.2";
                    else if (glewError == GLEW_ERROR_NO_GLX_DISPLAY)
                        str << "Need GLX Display for GLX support";

                    log::warning(str.str());
                }

                log::info("succesfully initialized OpenGL");
                detail::GL_INITIALIZED = true;
            }

            return GL_CONTEXT;
        }
    }

    namespace detail
    {
        DECLARE_NEW_TYPE(RenderAreaInternal, render_area_internal, RENDER_AREA_INTERNAL)

        static void render_area_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_RENDER_AREA_INTERNAL(object);
            G_OBJECT_CLASS(render_area_internal_parent_class)->finalize(object);

            for (auto* task : *self->tasks)
                g_object_unref(task);

            delete self->tasks;
        }

        DEFINE_NEW_TYPE_TRIVIAL_INIT(RenderAreaInternal, render_area_internal, RENDER_AREA_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(RenderAreaInternal, render_area_internal, RENDER_AREA_INTERNAL)

        static RenderAreaInternal* render_area_internal_new(GtkGLArea* area)
        {
            auto* self = (RenderAreaInternal*) g_object_new(render_area_internal_get_type(), nullptr);
            render_area_internal_init(self);

            self->native = area;
            self->tasks = new std::vector<detail::RenderTaskInternal*>();
            return self;
        }
    }

    RenderArea::RenderArea()
        : Widget(gtk_gl_area_new()),
          CTOR_SIGNAL(RenderArea, render),
          CTOR_SIGNAL(RenderArea, resize)
    {
        _internal = detail::render_area_internal_new(GTK_GL_AREA(operator NativeWidget()));
        detail::attach_ref_to(G_OBJECT(GTK_GL_AREA(operator NativeWidget())), _internal);
        g_object_ref(_internal);

        gtk_gl_area_set_auto_render(GTK_GL_AREA(operator NativeWidget()), TRUE);
        gtk_widget_set_size_request(GTK_WIDGET(GTK_GL_AREA(operator NativeWidget())), 1, 1);

        connect_signal("realize", on_realize, _internal);
        connect_signal("render", on_render, _internal);
        connect_signal("resize", on_resize, _internal);
        connect_signal("create-context", on_create_context, _internal);
    }

    RenderArea::~RenderArea()
    {
        g_object_unref(_internal);
    }

    RenderArea::RenderArea(detail::RenderAreaInternal* internal)
        : Widget(GTK_WIDGET(internal->native)),
          CTOR_SIGNAL(RenderArea, render),
          CTOR_SIGNAL(RenderArea, resize)
    {
        _internal = g_object_ref(internal);

        gtk_gl_area_set_auto_render(GTK_GL_AREA(operator NativeWidget()), TRUE);
        gtk_widget_set_size_request(GTK_WIDGET(GTK_GL_AREA(operator NativeWidget())), 1, 1);

        connect_signal("realize", on_realize, _internal);
        connect_signal("render", on_render, _internal);
        connect_signal("resize", on_resize, _internal);
        connect_signal("create-context", on_create_context, _internal);
    }

    void RenderArea::add_render_task(RenderTask task)
    {
        auto* task_internal = (detail::RenderTaskInternal*) task.operator GObject*();
        _internal->tasks->push_back(task_internal);
        g_object_ref(task_internal);
    }

    void RenderArea::clear_render_tasks()
    {
        for (auto& task : *_internal->tasks)
            g_object_unref(task);

        _internal->tasks->clear();
    }

    void RenderArea::flush()
    {
        glFlush();
    }

    void RenderArea::clear()
    {
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    GdkGLContext* RenderArea::on_create_context(GtkGLArea* area, GdkGLContext* context, detail::RenderAreaInternal*)
    {
        assert(detail::GL_INITIALIZED == true);
        gdk_gl_context_make_current(detail::GL_CONTEXT);
        return detail::GL_CONTEXT;
    }

    void RenderArea::on_realize(GtkWidget* area, detail::RenderAreaInternal* internal)
    {
        gtk_gl_area_queue_render(GTK_GL_AREA(area));
    }

    void RenderArea::on_resize(GtkGLArea* area, gint width, gint height, detail::RenderAreaInternal* internal)
    {
        gtk_gl_area_make_current(area);
        gtk_gl_area_queue_render(area);
    }

    gboolean RenderArea::on_render(GtkGLArea* area, GdkGLContext* context, detail::RenderAreaInternal* internal)
    {
        gtk_gl_area_make_current(area);

        RenderArea::clear();

        glEnable(GL_BLEND);
        set_current_blend_mode(BlendMode::NORMAL);

        for (auto* internal : *(internal->tasks))
        {
            auto task = RenderTask(internal);
            task.render();
        }

        RenderArea::flush();
        return TRUE;
    }

    void RenderArea::render_render_tasks()
    {
        for (auto* internal : *(_internal->tasks))
        {
            auto task = RenderTask(internal);
            task.render();
        }
    }

    void RenderArea::queue_render()
    {
        gtk_gl_area_queue_render(GTK_GL_AREA(operator NativeWidget()));
        gtk_widget_queue_draw(GTK_WIDGET(GTK_GL_AREA(operator NativeWidget())));
    }

    void RenderArea::make_current()
    {
        gtk_gl_area_make_current(GTK_GL_AREA(operator NativeWidget()));
    }

    Vector2f RenderArea::from_gl_coordinates(Vector2f in)
    {
        auto out = in;
        out /= 2;
        out += 0.5;
        out.y = 1 - out.y;

        auto size = this->get_allocated_size();
        return {out.x * size.x, out.y * size.y};
    }

    Vector2f RenderArea::to_gl_coordinates(Vector2f in)
    {
        auto out = in;

        auto size = this->get_allocated_size();
        out.x /= size.x;
        out.y /= size.y;

        out.y = 1 - out.y;
        out -= 0.5;
        out *= 2;

        return out;
    }

    GObject* RenderArea::get_internal() const
    {
        return G_OBJECT(_internal);
    }
}