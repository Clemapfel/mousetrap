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

        auto scrollbar = Scrollbar(Orientation::HORIZONTAL, Adjustment(0.5, 0, 1, 0.01));

        scrollbar.get_adjustment().connect_signal_value_changed([](Adjustment& adjustment){
            std::cout << adjustment.get_value() << std::endl;
        });

        window.set_child(scrollbar);
        window.present();
    });

    return app.run();
}
