//
// Created by clem on 4/12/23.
//

#include <mousetrap/application.hpp>
#include <mousetrap/window.hpp>
#include <mousetrap/label.hpp>
#include <mousetrap/motion_event_controller.hpp>
#include <mousetrap/box.hpp>
#include <mousetrap/drop_down.hpp>
#include <mousetrap/button.hpp>
#include <mousetrap/action.hpp>
#include <mousetrap/popover_menu.hpp>
#include <mousetrap/popover_button.hpp>
#include <mousetrap/list_view.hpp>

#include <mousetrap.hpp>

using namespace mousetrap;
#include <string>
#include <gtk/gtk.h>

GtkWidget * do_css_accordion (GtkWidget *do_widget) {

    static GtkWidget *window = gtk_window_new();
    auto* container = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);

    auto level_bar = LevelBar(0, 1);
    level_bar.set_value(0.5);
    level_bar.set_expand_horizontally(true);
    level_bar.set_size_request({200, 50});

    //gtk_box_append (GTK_BOX (container), gtk_button_new_with_label("||||||||||||||||"));
    gtk_box_append(GTK_BOX(container), level_bar.operator NativeWidget());
    gtk_window_set_child (GTK_WINDOW (window), container);
    gtk_widget_add_css_class (container, "accordion");


    auto* provider = gtk_css_provider_new();
    std::string css = R"(.accordion, .accordion * {
all: unset;

transition-property: color, background-color, border-color, background-image, padding, border-width;
transition-duration: 1s;

font: 20px Cantarell;
}

.accordion levelbar trough { background-color: magenta;border: 1px solid; padding: 2px; border-radius: 3px; color: white; border-color: #1b1b1b; background-color: #2d2d2d; box-shadow: inset 0 0 0 1px rgba(21, 83, 158, 0); }

.accordion levelbar trough:backdrop { color: #d6d6d6; border-color: #202020; background-color: #303030; box-shadow: none; }

.accordion levelbar block { background-color: magenta; border: 1px solid; border-radius: 1px; }



)";

    //gtk_css_provider_load_from_data(provider, css.c_str(), css.size());
    gtk_css_provider_load_from_resource(provider, "resource:///org/gtk/libgtk/theme/Default/Default-dark.css");
    gtk_css_provider_load_from_path(provider, "/home/clem/Workspace/mousetrap/gtk-contained.css");
    gtk_style_context_add_provider_for_display (gtk_widget_get_display (container), GTK_STYLE_PROVIDER (provider),

                                                GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    return window;
}

int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);
        auto* gtk_window = do_css_accordion(window.operator NativeWidget());
        gtk_window_present(GTK_WINDOW(gtk_window));
    });
    app.run();
}
