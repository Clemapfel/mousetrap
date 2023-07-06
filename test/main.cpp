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
    for (size_t i = 0; i < 3; ++i)
    {
        auto app = Application("test.app");
        app.connect_signal_activate([](Application& app)
        {
            auto window = Window(app);

            auto area = RenderArea();
            for (size_t i = 0; i < 3; ++i)
            {
                float x = 1 - rand() / float(RAND_MAX);
                float y = 1 - rand() / float(RAND_MAX);
                auto shape = Shape::Point({x, y});
                area.add_render_task(RenderTask(shape));
            }

            auto frame = AspectFrame(1.0);
            frame.set_child(area);
            frame.set_size_request({150, 150});
            window.set_child(frame);

            window.present();
        });

        app.run();
    }
}
