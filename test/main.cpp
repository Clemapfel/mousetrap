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

void temp()
{
    auto* settings = gtk_settings_get_for_display(gdk_display_get_default());
    //gtk_settings_reset_property(settings, "gtk-application-prefer-dark-theme");
    g_object_set(settings, "gtk-application-prefer-dark-theme", nullptr);
}

int main(int argc, char** argv)
{
    {
        static auto app = Application("test.id");
        app.connect_signal_activate([](Application& app)
        {
            static auto window = Window(app);
            auto button = Button();
            button.connect_signal_clicked([&](Button&){
                auto* style_manager = adw_style_manager_get_for_display(gtk_widget_get_display(window));
                if (adw_style_manager_get_dark(style_manager))
                    adw_style_manager_set_color_scheme(style_manager, ADW_COLOR_SCHEME_FORCE_LIGHT);
                else
                    adw_style_manager_set_color_scheme(style_manager, ADW_COLOR_SCHEME_FORCE_DARK);
            });
            window.set_is_decorated(false);
            window.set_child(button);
            window.present();

            auto* other_window = adw_window_new();
            gtk_window_set_application(GTK_WINDOW(other_window), app.operator GtkApplication *());
            auto vbox = Box(Orientation::VERTICAL);
            gtk_box_append(GTK_BOX(vbox.operator NativeWidget()), GTK_WIDGET(adw_header_bar_new()));
            vbox.push_back(Separator());
            adw_window_set_content(ADW_WINDOW(other_window), vbox);
            gtk_window_present(GTK_WINDOW(other_window));
        });
        app.run();
    }
}