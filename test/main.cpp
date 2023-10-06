#include <mousetrap.hpp>

using namespace mousetrap;

#include <mousetrap.hpp>

using namespace mousetrap;

#include <thread>
#include <chrono>

void changed() {
    std::cout << "called" << std::endl;
}

int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        static auto window = Window(app);
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

        static auto other_window = Window(app);
        auto button = Button();
        button.connect_signal_clicked([](Button& button){
            std::cout << window.get_is_closed() << std::endl;
        });
        other_window.set_child(button);
        other_window.present();
    });
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
