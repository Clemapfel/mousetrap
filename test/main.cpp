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

int main(int argc, char** argv)
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);
        window.connect_signal_close_request([](Window&){
            return WindowCloseRequestResult::ALLOW_CLOSE;
        });

        std::cout << GTK_IS_WIDGET(window.operator NativeObject()) << std::endl;
        std::cout << GTK_IS_WIDGET(window.operator NativeWidget()) << std::endl;
        window.present();
    });
    return app.run();
}