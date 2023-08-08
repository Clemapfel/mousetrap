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
        auto button = Button();

        static auto dialog = AlertDialog("test", "body");
        dialog.add_button("OK");
        dialog.add_button("Cancel");
        dialog.on_selection([](AlertDialog& self, int response){
            std::cout << self.get_button_label(response) << std::endl;
        });
        dialog.set_default_button(0);

        button.connect_signal_clicked([](Button&){
            dialog.present();
        });

        window.set_child(button);
        window.present();
    });
    return app.run();
}