//
// Created by clem on 7/31/22.
//

#include <include/gl_common.hpp>
#include <include/gtk_common.hpp>
#include <include/gl_area.hpp>
#include <include/components/color_picker.hpp>

using namespace mousetrap;

struct ShaderArea : public GLArea
{
    ShaderArea()
        : GLArea()
    {}

    void on_realize(GtkGLArea* area) override
    {
        gtk_gl_area_make_current(area);

        _shape = new Shape();
        _shape->as_rectangle({0.25, 0.25}, {0.5, 0.5});

        GLArea::add_render_task(_shape);
    }

    void on_shutdown(GtkGLArea*) {}

    Shape* _shape;
};

int main()
{
    gtk_init(nullptr, nullptr);

    // main window
    auto *main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);
    gtk_widget_realize(main_window);

    // init opengl
    gtk_initialize_opengl(GTK_WINDOW(main_window));

    // debug
    color_picker = new ColorPicker(150);
    gtk_container_add(GTK_CONTAINER(main_window), color_picker->get_native());

    // render loop
    gtk_widget_show_all(main_window);
    gtk_window_present((GtkWindow*) main_window);
    gtk_main();

    return 0;
}