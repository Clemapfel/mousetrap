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

static void

destroy_provider (GtkWidget        *window,

GtkStyleProvider *provider)

{

    gtk_style_context_remove_provider_for_display (gtk_widget_get_display (window), provider);

}

GtkWidget *

do_css_accordion (GtkWidget *do_widget)

{

    static GtkWidget *window = NULL;

    if (!window)

    {

        GtkWidget *container, *styled_box, *child;

        GtkCssProvider *provider;

        window = gtk_window_new ();

        gtk_window_set_title (GTK_WINDOW (window), "CSS Accordion");

        gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (do_widget));

        gtk_window_set_default_size (GTK_WINDOW (window), 600, 300);

        g_object_add_weak_pointer (G_OBJECT (window), (gpointer *)&window);

        styled_box = gtk_frame_new (NULL);

        gtk_window_set_child (GTK_WINDOW (window), styled_box);

        gtk_widget_add_css_class (styled_box, "accordion");

        container = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);

        gtk_widget_set_halign (container, GTK_ALIGN_CENTER);

        gtk_widget_set_valign (container, GTK_ALIGN_CENTER);

        gtk_frame_set_child (GTK_FRAME (styled_box), container);

        child = gtk_button_new_with_label ("This");

        gtk_box_append (GTK_BOX (container), child);

        child = gtk_button_new_with_label ("Is");

        gtk_box_append (GTK_BOX (container), child);

        child = gtk_button_new_with_label ("A");

        gtk_box_append (GTK_BOX (container), child);

        child = gtk_button_new_with_label ("CSS");

        gtk_box_append (GTK_BOX (container), child);

        child = gtk_button_new_with_label ("Accordion");

        gtk_box_append (GTK_BOX (container), child);

        child = gtk_button_new_with_label (":-)");

        gtk_box_append (GTK_BOX (container), child);

        provider = gtk_css_provider_new ();


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
        gtk_style_context_add_provider_for_display (gtk_widget_get_display (window), GTK_STYLE_PROVIDER (provider),

                                                    GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

        g_signal_connect (window, "destroy", G_CALLBACK (destroy_provider), provider);
        g_object_unref(provider);

    }

    if (!gtk_widget_get_visible (window))

        gtk_widget_set_visible (window, TRUE);

    else

        gtk_window_destroy (GTK_WINDOW (window));

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
