#include <mousetrap.hpp>

using namespace mousetrap;

#include <thread>
#include <chrono>


static void to_call(void* self, Application* app)
{
    std::cout << "called startup" << std::endl;
    auto action = Action("test.action", *app);
    auto model = MenuModel();
    auto sub = MenuModel();
    model.add_submenu("sub", sub);
    sub.add_action("test", action);
    gtk_application_set_menubar(GTK_APPLICATION(self), model.operator GMenuModel*());

    auto* window = gtk_application_window_new(app->operator GtkApplication *());
    gtk_window_present(GTK_WINDOW(window));
    gtk_application_window_set_show_menubar(GTK_APPLICATION_WINDOW(window), true);
}

int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        std::cout << "called activate" << std::endl;
        static auto window = Window(app);

        static auto area = RenderArea(AntiAliasingQuality::BEST);
        static auto shape = Shape::Rectangle({-0.5, 0.5}, {1, 1});
        shape.set_vertex_color(0, HSVA(0.1, 1, 1, 1));
        shape.set_vertex_color(1, HSVA(0.3, 1, 1, 1));
        shape.set_vertex_color(2, HSVA(0.6, 1, 1, 1));
        shape.set_vertex_color(3, HSVA(0.9, 1, 1, 1));

        area.add_render_task(RenderTask(shape));
        auto frame = AspectFrame(1.0);
        frame.set_child(area);
        frame.set_size_request({150, 150});

        static auto animation = Animation(area, seconds(3));
        animation.set_repeat_count(0);
        animation.on_tick([](Animation&, double value){
            static auto current = 0;
            shape.rotate(degrees(-1 * current), shape.get_centroid());
            shape.rotate(degrees(value * 360), shape.get_centroid());
            current = value * 360;
            area.queue_render();
        });

        area.connect_signal_map([](RenderArea& self){
            animation.play();
        });


        auto* settings = gtk_settings_get_default();
        auto* x = new GValue();
        g_value_init(x, G_TYPE_BOOLEAN);
        auto* y = new GValue();
        g_value_init(y, G_TYPE_BOOLEAN);

        g_object_get(G_OBJECT(settings),"gtk-shell-shows-app-menu", &x, "gtk-shell-shows-menubar", &y, NULL);
        std::cout << x << " " << y << std::endl;

        g_object_set(G_OBJECT(settings), "gtk-shell-shows-app-menu", true);
        g_object_set(G_OBJECT(settings), "gtk-shell-shows-menubar", true);

        window.set_child(frame);

        auto* label = gtk_shortcut_label_new("<Control>f");
        adw_application_window_set_content(ADW_APPLICATION_WINDOW(window.operator NativeWidget()), GTK_WIDGET(label));

        window.present();

        gtk_application_window_set_show_menubar(GTK_APPLICATION_WINDOW(window.operator GObject*()), true);
    });

    g_signal_connect(app.operator GObject*(), "startup", G_CALLBACK(to_call), &app);

    return app.run();
}


#if FALSE

#include <thread>
#include <chrono>

namespace mousetrap
{
    class MakieCanvas;
    namespace detail
    {
        struct _MakieCanvasInternal
        {
            GObject parent;
            GtkGLArea* area;
        };

        using MakieCanvasInternal = _MakieCanvasInternal;
        DEFINE_INTERNAL_MAPPING(MakieCanvas);

        DECLARE_NEW_TYPE(MakieCanvasInternal, makie_canvas_internal, MAKIE_CANVAS_INTERNAL)

        static void makie_canvas_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_MAKIE_CANVAS_INTERNAL(object);
            G_OBJECT_CLASS(makie_canvas_internal_parent_class)->finalize(object);
            g_object_unref(self->area);
        }

        DEFINE_NEW_TYPE_TRIVIAL_INIT(MakieCanvasInternal, makie_canvas_internal, MAKIE_CANVAS_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(MakieCanvasInternal, makie_canvas_internal, MAKIE_CANVAS_INTERNAL)

        static MakieCanvasInternal* makie_canvas_internal_new(GtkGLArea* area)
        {
            auto* self = (MakieCanvasInternal*) g_object_new(makie_canvas_internal_get_type(), nullptr);
            makie_canvas_internal_init(self);

            self->area = area;
            g_object_ref_sink(self->area);
            return self;
        }
    }

    class MakieCanvas :
        public detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(MakieCanvas, realize),
        HAS_SIGNAL(MakieCanvas, unrealize),
        HAS_SIGNAL(MakieCanvas, destroy),
        HAS_SIGNAL(MakieCanvas, hide),
        HAS_SIGNAL(MakieCanvas, show),
        HAS_SIGNAL(MakieCanvas, map),
        HAS_SIGNAL(MakieCanvas, unmap),
        HAS_SIGNAL(MakieCanvas, resize),
        HAS_SIGNAL(MakieCanvas, render)
    {
        private:
            detail::MakieCanvasInternal* _internal = nullptr;

        public:
            MakieCanvas()
            : Widget(gtk_gl_area_new()),
              CTOR_SIGNAL(MakieCanvas, realize),
              CTOR_SIGNAL(MakieCanvas, unrealize),
              CTOR_SIGNAL(MakieCanvas, destroy),
              CTOR_SIGNAL(MakieCanvas, hide),
              CTOR_SIGNAL(MakieCanvas, show),
              CTOR_SIGNAL(MakieCanvas, map),
              CTOR_SIGNAL(MakieCanvas, unmap),
              CTOR_SIGNAL(MakieCanvas, resize),
              CTOR_SIGNAL(MakieCanvas, render)
            {
                _internal = detail::makie_canvas_internal_new(GTK_GL_AREA(operator NativeWidget()));
                g_object_ref_sink(_internal);
            }

            MakieCanvas(detail::MakieCanvasInternal* internal)
                : Widget(GTK_WIDGET(internal->area)),
                  CTOR_SIGNAL(MakieCanvas, realize),
                  CTOR_SIGNAL(MakieCanvas, unrealize),
                  CTOR_SIGNAL(MakieCanvas, destroy),
                  CTOR_SIGNAL(MakieCanvas, hide),
                  CTOR_SIGNAL(MakieCanvas, show),
                  CTOR_SIGNAL(MakieCanvas, map),
                  CTOR_SIGNAL(MakieCanvas, unmap),
                  CTOR_SIGNAL(MakieCanvas, resize),
                  CTOR_SIGNAL(MakieCanvas, render)
            {
                _internal = g_object_ref(internal);
            }

            ~MakieCanvas()
            {
                g_object_unref(_internal);
            }
    };
}

int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app){
        auto window = Window(app);
        auto canvas = MakieCanvas();
        window.set_child(canvas);

        canvas.connect_signal_realize([](MakieCanvas& self){
            std::cout << "realize" << std::endl;
        });
        canvas.connect_signal_resize([](MakieCanvas& self, size_t x, size_t y){
            std::cout << "resize: " << x << " " << y << std::endl;
        });
        canvas.connect_signal_render([](MakieCanvas& self, GdkGLContext* context){
            static bool once = true;
            if (once)
            {
                once = false;
                std::cout << "render " << context << std::endl;
            }
            return true;
        });

        window.present();
    });
    return app.run();
}
    #endif
#if FALSE

void changed() {
    std::cout << "called" << std::endl;
}

int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);
        window.set_title("mousetrap");

        static auto area = RenderArea(AntiAliasingQuality::BEST);
        static auto shape = Shape::Rectangle({-0.5, 0.5}, {1, 1});
        shape.set_vertex_color(0, HSVA(0.1, 1, 1, 1));
        shape.set_vertex_color(1, HSVA(0.3, 1, 1, 1));
        shape.set_vertex_color(2, HSVA(0.6, 1, 1, 1));
        shape.set_vertex_color(3, HSVA(0.9, 1, 1, 1));

        area.add_render_task(RenderTask(shape));
        auto frame = AspectFrame(1.0);
        frame.set_child(area);
        frame.set_size_request({150, 150});

        static auto animation = Animation(area, seconds(3));
        animation.set_repeat_count(0);
        animation.on_tick([](Animation&, double value){
            static auto current = 0;
            shape.rotate(degrees(-1 * current), shape.get_centroid());
            shape.rotate(degrees(value * 360), shape.get_centroid());
            current = value * 360;
            area.queue_render();
        });

        area.connect_signal_map([](RenderArea& self){
           animation.play();
        });

        window.set_child(frame);
        window.present();
    });
    return app.run();
}
    #endif
