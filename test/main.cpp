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

        static auto render_area_off = RenderArea(AntiAliasingQuality::OFF);
        static auto render_area_on = RenderArea(AntiAliasingQuality::BEST);
        static auto shape = Shape::Circle({0, 0}, 0.75, 3);

        render_area_off.add_render_task(RenderTask(shape));
        render_area_on.add_render_task(RenderTask(shape));

        render_area_off.set_tick_callback([](FrameClock){
            shape.rotate(degrees(1), shape.get_centroid());
            render_area_off.queue_render();
            render_area_on.queue_render();
            return TickCallbackResult::CONTINUE;
        });

        auto frame_left = AspectFrame(1);
        frame_left.set_child(render_area_off);
        frame_left.set_size_request({150, 150});

        auto frame_right = AspectFrame(1);
        frame_right.set_child(render_area_on);
        frame_right.set_size_request({150, 150});

        auto paned = Paned(Orientation::HORIZONTAL);
        paned.set_start_child(frame_left);
        paned.set_end_child(frame_right);

        window.set_child(paned);
        window.present();
    });
    return app.run();
}