#include <mousetrap.hpp>

using namespace mousetrap;

#include <thread>
#include <chrono>

int main()
{
    // declare application
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);

        static auto action = Action("test.app", app);
        action.set_function([](Action&){
            std::cout << "called" << std::endl;
        });

        static auto* toast = adw_toast_overlay_new();
        auto button = Button();
        button.connect_signal_clicked([](Button&){

            static size_t i = 0;
            auto* msg = adw_toast_new((std::to_string(i++) + " Test").c_str());
            adw_toast_set_action_name(ADW_TOAST(msg), ("app." + action.get_id()).c_str());
            adw_toast_set_button_label(ADW_TOAST(msg), "Test");
            adw_toast_overlay_add_toast(ADW_TOAST_OVERLAY(toast), msg);
        });

        adw_toast_overlay_set_child(ADW_TOAST_OVERLAY(toast), button.operator NativeWidget());
        adw_bin_set_child(((detail::WindowInternal*) window.get_internal())->content_area, toast);

        window.present();
    });
    return app.run();
}
