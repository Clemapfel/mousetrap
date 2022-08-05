//
// Created by clem on 7/31/22.
//

#include <include/gl_common.hpp>
#include <include/gtk_common.hpp>
#include <include/gl_area.hpp>
#include <include/components/color_picker.hpp>
#include <include/components/hsv_triangle_select.hpp>
#include <include/window.hpp>

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

    // color picker
    color_picker = new ColorPicker(150);
    auto* color_picker_instance = (ColorPicker*) color_picker;
    color_picker_instance->set_size_request({200, 300});
    //gtk_container_add(GTK_CONTAINER(main_window), color_picker_instance->get_native());

    hsv_triangle_select = new HsvTriangleSelect();
    auto* hsv_triangle_select_instance = (HsvTriangleSelect*) hsv_triangle_select;
    gtk_container_add(GTK_CONTAINER(main_window), hsv_triangle_select_instance->get_native());
    hsv_triangle_select_instance->set_size_request({200, 200});

    // render loop
    gtk_widget_show_all(main_window);
    gtk_window_present((GtkWindow*) main_window);
    gtk_main();

    return 0;
}