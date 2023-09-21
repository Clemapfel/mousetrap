// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/gl_common.hpp>
#if MOUSETRAP_ENABLE_OPENGL_COMPONENT

#include <mousetrap/render_area.hpp>
#include <mousetrap/render_task.hpp>
#include <mousetrap/msaa_render_texture.hpp>
#include <mousetrap/shape.hpp>

namespace mousetrap
{
    namespace detail
    {
        GdkGLContext* initialize_opengl()
        {
            if (not mousetrap::GL_INITIALIZED)
            {
                GError* create_context_error = nullptr;
                GError* realize_context_error = nullptr;
                GLenum glew_error = 0;

                mousetrap::GL_INITIALIZED = true;

                auto* MOUSETRAP_DISABLE_OPENGL_COMPONENT = std::getenv("MOUSETRAP_DISABLE_OPENGL_COMPONENT");
                if (MOUSETRAP_DISABLE_OPENGL_COMPONENT != nullptr)
                {
                    auto gl_disabled = std::string(MOUSETRAP_DISABLE_OPENGL_COMPONENT);
                    if (gl_disabled == "1" or gl_disabled == "true" or gl_disabled == "TRUE" or gl_disabled == "yes" or gl_disabled == "YES" or gl_disabled == "on" or gl_disabled == "ON")
                    {
                        detail::GL_CONTEXT = nullptr;
                        log::warning("In initialize_opengl: OpenGL component disabled because `MOUSETRAP_DISABLED_OPENGL_COMPONENT` was set to `" + gl_disabled + "`", MOUSETRAP_DOMAIN);
                        return nullptr;
                    }
                    else if (not (gl_disabled == "0" or gl_disabled == "false" or gl_disabled == "FALSE" or gl_disabled == "no" or gl_disabled == "NO" or gl_disabled == "ON" or gl_disabled == "OFF"))
                    {
                        log::critical("In initialize_opengl: ignoring value of environment variable `MOUSETRAP_DISABLED_OPENGL_COMPONENT`, because it is malformed. Expected `TRUE` or `FALSE`, got `" + gl_disabled + "`", MOUSETRAP_DOMAIN);
                    }
                }

                auto* display = gdk_display_get_default();
                if (display == nullptr)
                {
                    log::warning("In gdk_display_get_default: Unable to access default dispay.", MOUSETRAP_DOMAIN);
                    goto failed;
                }

                detail::GL_CONTEXT = gdk_display_create_gl_context(display, &create_context_error);

                if (create_context_error != nullptr)
                {
                    auto message = create_context_error->message;
                    log::warning(std::string("In gdk_window_create_gl_context:") + (message == nullptr ? "(unknown error)" : message), MOUSETRAP_DOMAIN);
                    g_error_free(create_context_error);
                    goto failed;
                }

                if (detail::GL_CONTEXT == nullptr)
                {
                    log::warning("In initialize_opengl: Unable to create an OpenGL context.", MOUSETRAP_DOMAIN);
                    goto failed;
                }

                gdk_gl_context_set_required_version(detail::GL_CONTEXT, 3, 3);
                gdk_gl_context_realize(detail::GL_CONTEXT, &realize_context_error);

                if (realize_context_error != nullptr)
                {
                    auto message = realize_context_error->message;
                    log::warning(std::string("In gdk_gl_context_realize:") + (message == nullptr ? "(unknown error)" : message), MOUSETRAP_DOMAIN);
                    g_error_free(realize_context_error);
                    goto failed;
                }

                gdk_gl_context_make_current(detail::GL_CONTEXT);
                glewExperimental = GL_FALSE;
                glew_error = glewInit();

                if (glew_error != GLEW_NO_ERROR)
                {
                    std::stringstream str;
                    str << "In glewInit: Unable to initialize glew " << "(" << glew_error << ") ";

                    if (glew_error == GLEW_ERROR_NO_GL_VERSION)
                        str << "Missing GL version";
                    else if (glew_error == GLEW_ERROR_GL_VERSION_10_ONLY)
                        str << "Need at least OpenGL 1.1";
                    else if (glew_error == GLEW_ERROR_GLX_VERSION_11_ONLY)
                        str << "Need at least GLX 1.2";
                    else if (glew_error == GLEW_ERROR_NO_GLX_DISPLAY)
                        str << "Need GLX Display for GLX support";

                    log::warning(str.str(), MOUSETRAP_DOMAIN);
                    goto failed;
                }

                g_object_ref_sink(GL_CONTEXT);
                g_object_ref(GL_CONTEXT);
                return detail::GL_CONTEXT;

                failed:
                log::critical("In initialize_opengl: Unable to create OpenGL context, disabling the OpenGL component.", MOUSETRAP_DOMAIN);
                detail::GL_CONTEXT = nullptr;
            }

            return detail::GL_CONTEXT;
        }

        void shutdown_opengl()
        {
            while (GDK_IS_GL_CONTEXT(GL_CONTEXT))
                g_object_unref(GL_CONTEXT);

            GL_CONTEXT = nullptr;
            GL_INITIALIZED = false;
        }

        bool is_opengl_disabled()
        {
            return (mousetrap::GL_INITIALIZED == false or detail::GL_CONTEXT == nullptr);
        }
    }

    namespace detail
    {
        DECLARE_NEW_TYPE(RenderAreaInternal, render_area_internal, RENDER_AREA_INTERNAL)

        static void render_area_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_RENDER_AREA_INTERNAL(object);
            G_OBJECT_CLASS(render_area_internal_parent_class)->finalize(object);

            if (detail::is_opengl_disabled())
                return;

            for (auto* task : *self->tasks)
                g_object_unref(task);

            delete self->tasks;
            delete self->render_texture;
            delete self->render_texture_shape;
            delete self->render_texture_shape_task;
        }

        DEFINE_NEW_TYPE_TRIVIAL_INIT(RenderAreaInternal, render_area_internal, RENDER_AREA_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(RenderAreaInternal, render_area_internal, RENDER_AREA_INTERNAL)

        static RenderAreaInternal* render_area_internal_new(GtkGLArea* area, int32_t msaa_samples)
        {
            auto* self = (RenderAreaInternal*) g_object_new(render_area_internal_get_type(), nullptr);
            render_area_internal_init(self);

            if (detail::is_opengl_disabled())
            {
                log::critical("In render_area_internal_new: Trying to instantiate mousetrap::RenderArea, but the OpenGL component is disabled", MOUSETRAP_DOMAIN);
                return self;
            }

            self->native = area;
            self->tasks = new std::vector<detail::RenderTaskInternal*>();
            self->apply_msaa = msaa_samples > 0;

            if (self->apply_msaa)
            {
                self->render_texture = new MultisampledRenderTexture(msaa_samples);

                self->render_texture_shape = new Shape();
                self->render_texture_shape->as_rectangle({-1, 1}, {2, 2});
                self->render_texture_shape->set_texture(self->render_texture);

                static const std::string RENDER_TEXTURE_SHADER_SOURCE = R"(
                    #version 130

                    in vec4 _vertex_color;
                    in vec2 _texture_coordinates;
                    in vec3 _vertex_position;

                    out vec4 _fragment_color;

                    uniform int _texture_set;
                    uniform sampler2D _texture;

                    void main()
                    {
                        // flip horizontally to correct render texture inversion
                        _fragment_color = texture2D(_texture, vec2(_texture_coordinates.x, 1 - _texture_coordinates.y)) * _vertex_color;
                    }
                )";

                self->render_texture_shader = new Shader();
                self->render_texture_shader->create_from_string(ShaderType::FRAGMENT, RENDER_TEXTURE_SHADER_SOURCE);

                self->render_texture_shape_task = new RenderTask(*self->render_texture_shape, self->render_texture_shader);
            }
            else
            {
                self->render_texture = nullptr;
                self->render_texture_shape = nullptr;
                self->render_texture_shape_task = nullptr;
            }

            return self;
        }
    }

    RenderArea::RenderArea(AntiAliasingQuality msaa_samples)
        : Widget(gtk_gl_area_new()),
          CTOR_SIGNAL(RenderArea, render),
          CTOR_SIGNAL(RenderArea, resize),
          CTOR_SIGNAL(RenderArea, realize),
          CTOR_SIGNAL(RenderArea, unrealize),
          CTOR_SIGNAL(RenderArea, destroy),
          CTOR_SIGNAL(RenderArea, hide),
          CTOR_SIGNAL(RenderArea, show),
          CTOR_SIGNAL(RenderArea, map),
          CTOR_SIGNAL(RenderArea, unmap)
    {
        if (detail::is_opengl_disabled())
        {
            _internal = nullptr;
            return;
        }

        _internal = detail::render_area_internal_new(GTK_GL_AREA(operator NativeWidget()), (int) msaa_samples);
        detail::attach_ref_to(G_OBJECT(_internal->native), _internal);

        gtk_gl_area_set_auto_render(GTK_GL_AREA(operator NativeWidget()), TRUE);
        gtk_widget_set_size_request(GTK_WIDGET(GTK_GL_AREA(operator NativeWidget())), 1, 1);

        g_signal_connect(_internal->native, "realize", G_CALLBACK(on_realize), _internal);
        g_signal_connect(_internal->native, "resize", G_CALLBACK(on_resize), _internal);
        g_signal_connect(_internal->native, "render", G_CALLBACK(on_render), _internal);
        g_signal_connect(_internal->native, "create-context", G_CALLBACK(on_create_context), _internal);
    }

    RenderArea::~RenderArea()
    {}

    RenderArea::RenderArea(detail::RenderAreaInternal* internal)
        : Widget(internal == nullptr ? gtk_gl_area_new() : GTK_WIDGET(internal->native)),
          CTOR_SIGNAL(RenderArea, render),
          CTOR_SIGNAL(RenderArea, resize),
          CTOR_SIGNAL(RenderArea, realize),
          CTOR_SIGNAL(RenderArea, unrealize),
          CTOR_SIGNAL(RenderArea, destroy),
          CTOR_SIGNAL(RenderArea, hide),
          CTOR_SIGNAL(RenderArea, show),
          CTOR_SIGNAL(RenderArea, map),
          CTOR_SIGNAL(RenderArea, unmap)
    {
        if (detail::is_opengl_disabled())
        {
            _internal = nullptr;
            return;
        }

        _internal = g_object_ref(internal);

        gtk_gl_area_set_auto_render(GTK_GL_AREA(operator NativeWidget()), TRUE);
        gtk_widget_set_size_request(GTK_WIDGET(GTK_GL_AREA(operator NativeWidget())), 1, 1);

        g_signal_connect(_internal->native, "realize", G_CALLBACK(on_realize), _internal);
        g_signal_connect(_internal->native, "resize", G_CALLBACK(on_resize), _internal);
        g_signal_connect(_internal->native, "render", G_CALLBACK(on_render), _internal);
        g_signal_connect(_internal->native, "create-context", G_CALLBACK(on_create_context), _internal);
    }

    void RenderArea::add_render_task(RenderTask task)
    {
        if (detail::is_opengl_disabled())
            return;

        auto* task_internal = (detail::RenderTaskInternal*) task.operator GObject*();
        _internal->tasks->push_back(task_internal);
        g_object_ref(task_internal);
    }

    void RenderArea::clear_render_tasks()
    {
        if (detail::is_opengl_disabled())
            return;

        for (auto& task : *_internal->tasks)
            g_object_unref(task);

        _internal->tasks->clear();
    }

    void RenderArea::flush()
    {
        if (detail::is_opengl_disabled())
            return;

        glFlush();
    }

    void RenderArea::clear()
    {
        if (detail::is_opengl_disabled())
            return;

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0, 0, 0, 0);
    }

    GdkGLContext* RenderArea::on_create_context(GtkGLArea* area, GdkGLContext* context, detail::RenderAreaInternal* internal)
    {
        if (detail::is_opengl_disabled())
            return nullptr;

        detail::initialize_opengl();
        g_object_ref(detail::GL_CONTEXT);
        gdk_gl_context_make_current(detail::GL_CONTEXT);
        return detail::GL_CONTEXT;
    }

    void RenderArea::on_realize(GtkWidget* area, detail::RenderAreaInternal* internal)
    {
        if (detail::is_opengl_disabled())
            return;

        gtk_gl_area_queue_render(GTK_GL_AREA(area));
    }

    void RenderArea::on_resize(GtkGLArea* area, gint width, gint height, detail::RenderAreaInternal* internal)
    {
        if (detail::is_opengl_disabled())
            return;

        assert(GDK_IS_GL_CONTEXT(detail::GL_CONTEXT));

        if (internal->apply_msaa)
            internal->render_texture->create(width, height);

        gtk_gl_area_make_current(area);
        gtk_gl_area_queue_render(area);
    }

    gboolean RenderArea::on_render(GtkGLArea* area, GdkGLContext* context, detail::RenderAreaInternal* internal)
    {
        if (detail::is_opengl_disabled())
            return FALSE;

        assert(GDK_IS_GL_CONTEXT(detail::GL_CONTEXT));
        gtk_gl_area_make_current(area);

        if (internal->apply_msaa)
        {
            internal->render_texture->bind_as_render_target();

            RenderArea::clear();
            glEnable(GL_BLEND);
            set_current_blend_mode(BlendMode::NORMAL);

            for (auto* internal:*(internal->tasks))
                RenderTask(internal).render();

            RenderArea::flush();

            internal->render_texture->unbind_as_render_target();

            RenderArea::clear();
            glEnable(GL_BLEND);
            set_current_blend_mode(BlendMode::NORMAL);

            internal->render_texture_shape_task->render();
            RenderArea::flush();
        }
        else
        {
            RenderArea::clear();

            glEnable(GL_BLEND);
            set_current_blend_mode(BlendMode::NORMAL);

            for (auto* internal:*(internal->tasks))
                RenderTask(internal).render();

            RenderArea::flush();
        }

        return TRUE;
    }

    void RenderArea::render_render_tasks()
    {
        if (detail::is_opengl_disabled())
            return;

        for (auto* internal : *(_internal->tasks))
        {
            auto task = RenderTask(internal);
            task.render();
        }
    }

    void RenderArea::queue_render()
    {
        if (detail::is_opengl_disabled())
            return;

        gtk_gl_area_queue_render(GTK_GL_AREA(operator NativeWidget()));
        gtk_widget_queue_draw(GTK_WIDGET(GTK_GL_AREA(operator NativeWidget())));
    }

    void RenderArea::make_current()
    {
        if (detail::is_opengl_disabled())
            return;

        gtk_gl_area_make_current(GTK_GL_AREA(operator NativeWidget()));
    }

    Vector2f RenderArea::from_gl_coordinates(Vector2f in)
    {
        if (detail::is_opengl_disabled())
            return {0, 0};

        auto out = in;
        out /= 2;
        out += 0.5;
        out.y = 1 - out.y;

        auto size = this->get_allocated_size();
        return {out.x * size.x, out.y * size.y};
    }

    Vector2f RenderArea::to_gl_coordinates(Vector2f in)
    {
        if (detail::is_opengl_disabled())
            return {0, 0};

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
        if (detail::is_opengl_disabled())
            return nullptr;

        return G_OBJECT(_internal);
    }
}

#endif // MOUSETRAP_ENABLE_OPENGL_COMPONENT