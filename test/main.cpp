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
    auto* child = gtk_button_new_with_label("test");

    gtk_box_append (GTK_BOX (container), child);
    gtk_window_set_child (GTK_WINDOW (window), container);
    gtk_widget_add_css_class (container, "accordion");


    auto* provider = gtk_css_provider_new();
    std::string css = R"(.accordion, .accordion * {
all: unset;

transition-property: color, background-color, border-color, background-image, padding, border-width;
transition-duration: 1s;

font: 20px Cantarell;
}

.accordion {
background: linear-gradient(153deg, #151515, #151515 5px, transparent 5px) 0 0,
            linear-gradient(333deg, #151515, #151515 5px, transparent 5px) 10px 5px,
            linear-gradient(153deg, #222, #222 5px, transparent 5px) 0 5px,
            linear-gradient(333deg, #222, #222 5px, transparent 5px) 10px 10px,
            linear-gradient(90deg, #1b1b1b, #1b1b1b 10px, transparent 10px),
            linear-gradient(#1d1d1d, #1d1d1d 25%, #1a1a1a 25%, #1a1a1a 50%, transparent 50%, transparent 75%, #242424 75%, #242424);
background-color: #131313;
background-size: 20px 20px;
}

.accordion button {
color: black;
background-color: #bbb;
border-style: solid;
border-width: 2px 0 2px 2px;
border-color: #333;

padding: 12px 4px;
}

.accordion button:first-child {
border-radius: 5px 0 0 5px;
}

.accordion button:last-child {
border-radius: 0 5px 5px 0;
border-width: 2px;
}

.accordion button:hover {
padding: 12px 48px;
background-color: #4870bc;
}

.accordion button *:hover {
color: white;
}

.accordion button:hover:active,
.accordion button:active {
background-color: #993401;
}
)";

    gtk_css_provider_load_from_data(provider, css.c_str(), css.size());
    gtk_style_context_add_provider_for_display (gtk_widget_get_display (child), GTK_STYLE_PROVIDER (provider),

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
