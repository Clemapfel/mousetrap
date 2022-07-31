//
// Created by clem on 7/31/22.
//

#include <include/gl_common.hpp>
#include <include/gtk_common.hpp>
#include <include/gl_canvas.hpp>

using namespace rat;

struct ShaderArea : public GLCanvas
{
    ShaderArea()
        : GLCanvas({100, 100})
    {}

    void on_realize(GtkGLArea* area) override
    {
        gtk_gl_area_make_current(area);

        _shape = new Shape();
        _shape->as_rectangle({0.25, 0.25}, {0.5, 0.5});

        GLCanvas::register_render_task(_shape);
    }

    void on_shutdown(GtkGLArea*) {}

    Shape* _shape;
};

int main()
{
    gtk_init(nullptr, nullptr);

    // main window
    auto *main = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *main_window = main;

    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);
    gtk_widget_realize(main_window);

    // init opengl
    gtk_initialize_opengl(GTK_WINDOW(main_window));

    auto* gl_area = new ShaderArea();
    gtk_container_add(GTK_CONTAINER(main_window), gl_area->get_native());


    // render loop
    gtk_widget_show_all(main_window);
    gtk_window_present((GtkWindow*) main_window);
    gtk_main();

    return 0;
}