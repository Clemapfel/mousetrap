#include <mousetrap.hpp>
#include <iostream>
using namespace mousetrap;

// equivalent to Julia `println`, for debugging
template<typename... Arg_ts>
void println(Arg_ts... args)
{
    // don't bother trying to understand how this works, C++ is awful like that
    auto print = [](auto x){ std::cout << x; };
    (print(args), ...);
    std::cout << std::endl;
}

// convenience function to set GtkSettings value, for usage, see `GTK4 CODE BEGIN` below
template<typename Arg_t>
void gtk_settings_set(const std::string& name, Arg_t value)
{
    auto* settings = gtk_settings_get_default();
    g_object_set(G_OBJECT(settings), name.c_str(), value, NULL);
}

// convenience function to access GtkSettings value, booleans
bool gtk_settings_get_bool(const std::string& setting_id)
{
    auto* settings = gtk_settings_get_default();
    gboolean out = false;
    g_object_get(G_OBJECT(settings), setting_id.c_str(), &out, NULL);
    return out;
}

// convenience function to access GtkSettings value, string
std::string gtk_settings_get_string(const std::string& setting_id)
{
    auto* settings = gtk_settings_get_default();
    gchar* out = nullptr;
    g_object_get(G_OBJECT(settings), setting_id.c_str(), &out, NULL);
    return std::string(out == nullptr ? "" : out);
}

// convenience function to access GtkSettings value, number
int64_t gtk_settings_get_number(const std::string& setting_id)
{
    auto* settings = gtk_settings_get_default();
    gint out = -1;
    g_object_get(G_OBJECT(settings), setting_id.c_str(), &out, NULL);
    return int64_t(out);
}

static Window* main_window = nullptr;

void on_startup(void*, Application* app_ptr)
{
    auto& app = *app_ptr;
    main_window = new Window(app);

    // create window
    Window& window = *main_window;

    // create dummy actions
    auto action_01 = Action("dummy.action_01", app);
    auto action_02 = Action("dummy.action_02", app);

    // action behavior, cf. Julia `set_function!`
    action_01.set_function([](Action& self){
        println("called 01");
    });

    action_02.set_function([](Action& self){
        println("called 02");
    });

    // action shortcut, cf. Julia `add_shortcut!`
    action_01.add_shortcut("<Control>1");
    action_02.add_shortcut("<Control>2");

    // create outer menu model
    auto outer = MenuModel();

    // create inner menu models with one action each, cf. Julia `add_action!`
    auto inner_01 = MenuModel();
    inner_01.add_action("Action 01", action_01);

    auto inner_02 = MenuModel();
    inner_02.add_action("Action 02", action_02);

    // add inner models as submenus, cf. Julia `add_submenu!`
    outer.add_submenu("Inner 01", inner_01);
    outer.add_submenu("Inner 02", inner_02);

    // GTK4 CODE BEGIN
    {
        // get native menu model, cf. https://docs.gtk.org/gio/class.MenuModel.html
        GMenuModel* outer_native_ptr = G_MENU_MODEL(outer.operator GObject*());

        // get native application object, cf. https://docs.gtk.org/gtk4/class.Application.html
        GtkApplication* app_native_ptr = GTK_APPLICATION(app.operator GObject*());

        // get native window object, cf. https://docs.gtk.org/gtk4/class.ApplicationWindow.html
        GtkApplicationWindow* window_native_ptr = GTK_APPLICATION_WINDOW(window.operator GObject*());

        // associate a menu model with the applications default menu
        gtk_application_set_menubar(app_native_ptr, outer_native_ptr);

        // set menubar-related settings, try changing these to different values
        //gtk_settings_set("gtk-shell-shows-app-menu", true);
        //gtk_settings_set("gtk-shell-shows-menubar", true);

        // set whether the menubar should be displayed by the window
        gtk_application_window_set_show_menubar(window_native_ptr, true);

        // print variables for debugging, be sure to write these down when the menu ends up being displayed properly
        println("`gtk-shell-shows-app-menu`:\t", gtk_settings_get_bool("gtk-shell-shows-app-menu"));
        println("`gtk-shell-shows-menubar`:\t", gtk_settings_get_bool("gtk-shell-shows-menubar"));
        println("`Application.show_menubar`:\t", gtk_application_window_get_show_menubar(window_native_ptr));
    }
    // GTK4 CODE END
}

// C main loop
int main()
{
    // create application
    auto app = Application("com.mousetrap.apple_test");

    app.connect_signal("startup", on_startup, &app);
    app.connect_signal_activate([](Application& app){
        main_window->present();
    });

    // start application
    return app.run();
}