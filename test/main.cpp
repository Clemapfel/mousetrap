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
            auto window = Window(app);
            auto button = Button();
            button.connect_signal_clicked([&](Button&){
               auto current_theme = app.get_current_theme();
               if (current_theme == Theme::DEFAULT_DARK)
                   app.set_current_theme(Theme::DEFAULT_LIGHT);
               else
                   app.set_current_theme(Theme::DEFAULT_DARK);
            });

            auto box = Box(Orientation::HORIZONTAL);
            box.push_back(button);
            box.push_back(Button());
            box.push_back(HeaderBar());
            window.set_child(box);
            window.present();
        });
        app.run();
    }
}