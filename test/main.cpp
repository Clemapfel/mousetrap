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


int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);
        window.set_child( Label("Hello World"));
        window.present();
    });

    return app.run();
}
