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

        auto list = ListView(Orientation::VERTICAL, SelectionMode::MULTIPLE);
        list.push_back(Label("01"));
        list.push_back(Label("02"));
        list.push_back(Label("03"));
        list.push_back(Label("04"));
        list.push_back(Label("05"));

        list.get_selection_model()->connect_signal_selection_changed([](SelectionModel&, int32_t i, int32_t n){
            std::cout << i << " " << n << std::endl;
        });

        static auto render_area = RenderArea();
        render_area.set_expand(true);
        render_area.set_size_request({150, 150});

        // create shape
        // recall that an ellipse for whom equal x- and y-radius is identical to a circle
        static auto circle = Shape::Ellipse({0, 0}, 0.5, 0.5, 32);
        render_area.add_render_task(RenderTask(circle));

        // react to render area changing size
        render_area.connect_signal_resize([](RenderArea& area, int32_t width, int32_t height)
          {
            std::cout << width << " " << height << std::endl;

              // calculate y-to-x-ratio
              float new_ratio = float(height) / width;

              // multiply x-radius to normalize it. Then reformat the `circle` shape
              circle.as_ellipse({0, 0}, 0.5 * new_ratio,  0.5, 32);

              // force an update
              area.queue_render();
          });

        window.set_child(render_area);
        window.present();
    });
    return app.run();
}