//
// Created by clem on 7/31/22.
//

#include <include/gl_common.hpp>
#include <include/gtk_common.hpp>
#include <include/gl_area.hpp>
#include <include/components/color_picker.hpp>
#include <include/components/hsv_triangle_select.hpp>

using namespace mousetrap;


int main()
{
    gtk_init(nullptr, nullptr);

    // main window
    auto *main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);

    // init opengl
    gtk_widget_realize(main_window);
    gtk_initialize_opengl(GTK_WINDOW(main_window));

    // debug button
    Button button;
    button.set_valign(GTK_ALIGN_END);
    button.set_halign(GTK_ALIGN_END);
    //button.connect_signal("clicked", button_aux);

    auto overlay = Overlay();
    overlay.set_over(button);

    // debug
    color_picker = new ColorPicker(150);
    hsv_triangle_select = new HsvTriangleSelect();
    ((HsvTriangleSelect*) hsv_triangle_select)->set_size_request({300, 300});

    //
    // overlay.set_under((ColorPicker*) color_picker);
    overlay.set_under((HsvTriangleSelect*) hsv_triangle_select);
    gtk_container_add(GTK_CONTAINER(main_window), overlay);

    // render loop
    gtk_widget_show_all(main_window);
    gtk_window_present((GtkWindow*) main_window);
    gtk_main();

    return 0;
}