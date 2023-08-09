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
using namespace mousetrap;
int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);

        auto content = Box(Orientation::VERTICAL);

        auto image = Image(150, 150, RGBA(1, 0, 1, 1));
        auto image_display = ImageDisplay();
        image_display.create_from_image(image);
        image_display.set_expand(true);
        image_display.set_size_request({0, 100});

        static auto revealer = Revealer();
        revealer.set_child(image_display);
        revealer.set_transition_duration(seconds(1.0));
        revealer.set_transition_type(RevealerTransitionType::SLIDE_DOWN);
        revealer.set_is_revealed(false);
        revealer.set_expand_vertically(true);

        auto button = Button();
        button.set_child(Label("Click to Reveal"));
        button.set_expand_vertically(false);
        button.connect_signal_clicked([](Button& button){
            revealer.set_is_revealed(not revealer.get_is_revealed());
        });

        content.push_back(button);
        content.push_back(revealer);

        window.set_child(content);
        window.present();
    });
    return app.run();
}