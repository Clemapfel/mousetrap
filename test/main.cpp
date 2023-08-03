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

        static auto render_area = RenderArea(AntiAliasingQuality::GOOD);
        static auto shape = Shape::Circle({0, 0}, 0.75, 3);

        render_area.add_render_task(RenderTask(shape));
        render_area.set_tick_callback([](FrameClock){
            shape.rotate(degrees(1), shape.get_centroid());
            render_area.queue_render();
            return TickCallbackResult::CONTINUE;
        });

        auto frame = AspectFrame(1);
        frame.set_child(render_area);
        window.set_child(frame);
        window.present();
    });
    return app.run();
}