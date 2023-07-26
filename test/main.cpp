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

#include <thread>

#include <mousetrap.hpp>

using namespace mousetrap;

static void on_css_provider_parsing_error(GtkCssProvider* self, GtkCssSection* section, GError* error, void*)
{
    std::cout << gtk_css_section_to_string(section) << std::endl;

    if (error != nullptr)
        std::cout << error->message << std::endl;
}

int main()
{
    {
        auto app = Application("test.id");
        app.connect_signal_activate([](Application& app)
        {
            auto* settings = gtk_settings_get_for_display(gdk_display_get_default());

            auto* provider = gtk_css_provider_new();
            //gtk_css_provider_load_named(provider, "Adwaita", "dark");
            gtk_css_provider_load_from_path(provider, "/home/clem/Workspace/gtk-gtk-4.0/gtk/theme/HighContrast/theme.css");
            gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
            g_signal_connect(provider, "parsing-error", G_CALLBACK(on_css_provider_parsing_error), nullptr);

            /*
            g_object_set(settings, "gtk-application-prefer-dark-theme", false, NULL);
            g_object_set(settings, "gtk-theme-name", "Adwaita-light", NULL);

            const char* theme = nullptr;
            g_object_get(settings, "gtk-theme-name", &theme, NULL);
            std::cout << theme << std::endl;
            */
            auto window = Window(app);

            auto box = Box(Orientation::HORIZONTAL);

            auto button = Button();
            box.push_back(button);
            box.push_back(button);

            window.set_child(button);
            window.set_child(Scale(0, 1, 0.01));
            window.present();
        });
        app.run();
    }

    {
        auto app = Application("test.id");
        app.connect_signal_activate([](Application& app)
        {
            auto* settings = gtk_settings_get_for_display(gdk_display_get_default());

            auto* provider = gtk_css_provider_new();
            //gtk_css_provider_load_named(provider, "Adwaita", "dark");
            gtk_css_provider_load_from_path(provider, "/home/clem/Workspace/gtk-gtk-4.0/gtk/theme/Adwaita/adwaita.css");
            gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
            g_signal_connect(provider, "parsing-error", G_CALLBACK(on_css_provider_parsing_error), nullptr);

            /*
            g_object_set(settings, "gtk-application-prefer-dark-theme", false, NULL);
            g_object_set(settings, "gtk-theme-name", "Adwaita-light", NULL);

            const char* theme = nullptr;
            g_object_get(settings, "gtk-theme-name", &theme, NULL);
            std::cout << theme << std::endl;
            */
            auto window = Window(app);

            auto button = Button();
            window.set_child(Scale(0, 1, 0.01));
            window.present();
        });
        app.run();
    }
}

#if FALSE

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
    for (size_t i = 0; i < 5; ++i)
    {
        auto app = Application("test.app");
        app.connect_signal_activate([](Application& app)
        {
            auto window = Window(app);

            auto render_area = RenderArea();
            render_area.add_render_task(RenderTask(Shape::Rectangle({-0.5, 0.5}, {1, 1})));

            auto aspect_frame = AspectFrame(1.0);
            assert(aspect_frame.get_child_x_alignment() == 0.5);
            assert(aspect_frame.get_child_y_alignment() == 0.5);

            std::cout << aspect_frame.get_child_x_alignment() << " " << aspect_frame.get_child_y_alignment() << std::endl;

            aspect_frame.set_child_x_alignment(0.75);
            aspect_frame.set_child_y_alignment(0.75);

            std::cout << aspect_frame.get_child_x_alignment() << " " << aspect_frame.get_child_y_alignment() << std::endl;

            assert(aspect_frame.get_child_x_alignment() == 0.75);
            assert(aspect_frame.get_child_y_alignment() == 0.75);

            aspect_frame.set_size_request({150, 150});
            aspect_frame.set_child(render_area);

            window.set_child(aspect_frame);
            window.present();
        });
        app.run();
    }
}
#endif