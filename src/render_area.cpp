// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/render_area.hpp>

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

            delete self->tasks;
        }

        DEFINE_NEW_TYPE_TRIVIAL_INIT(RenderAreaInternal, render_area_internal, RENDER_AREA_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(RenderAreaInternal, render_area_internal, RENDER_AREA_INTERNAL)

        static RenderAreaInternal* render_area_internal_new()
        {
            auto* self = (RenderAreaInternal*) g_object_new(render_area_internal_get_type(), nullptr);
            render_area_internal_init(self);

            self->tasks = new std::vector<RenderTask>();
            return self;
        }
    }

    RenderArea::RenderArea()
        : WidgetImplementation<GtkGLArea>(GTK_GL_AREA(gtk_gl_area_new())),
          CTOR_SIGNAL(RenderArea, render),
          CTOR_SIGNAL(RenderArea, resize)
    {
        _internal = detail::render_area_internal_new();
        detail::attach_ref_to(G_OBJECT(get_native()), _internal);

        gtk_gl_area_set_auto_render(get_native(), TRUE);
        gtk_widget_set_size_request(GTK_WIDGET(get_native()), 1, 1);

        connect_signal_render(on_render);
        connect_signal_resize(on_resize);

        connect_signal("create-context", on_create_context);
    }

    void RenderArea::add_render_task(const Shape& shape, Shader* shader, GLTransform* transform, BlendMode blend_mode)
    {
        if (shape == nullptr)
            return;

        _internal->tasks->emplace_back(shape, shader, transform, blend_mode);
    }

    void RenderArea::add_render_task(RenderTask task)
    {
        _internal->tasks->push_back(task);
    }

    void RenderArea::clear_render_tasks()
    {
        _internal->tasks->clear();
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

        for (auto& task : *(area->_internal->tasks))
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

    GdkGLContext* RenderArea::on_create_context(GtkGLArea* area, GdkGLContext* context)
    {
        assert(detail::GL_INITIALIZED == true);
        gdk_gl_context_make_current(detail::GL_CONTEXT);
        return detail::GL_CONTEXT;
    }
}