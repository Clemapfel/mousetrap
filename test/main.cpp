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

struct ShapePage : public CompoundWidget
{
    RenderArea render_area;
    AspectFrame aspect_frame;
    Label label;
    CenterBox center_box;

    Widget& as_widget() override
    {
        return center_box;
    }

    ShapePage(const std::string& title, Shape shape)
        : render_area(),
          aspect_frame(1.0),
          label(title),
          center_box(Orientation::VERTICAL)
    {
        render_area.add_render_task(RenderTask(shape));
        aspect_frame.set_child(render_area);
        aspect_frame.set_size_request({150, 150});
        center_box.set_start_child(aspect_frame);
        center_box.set_end_child(label);
    }
};

void add_page(Stack& stack, const std::string& title, const Shape& shape)
{
    stack.add_child(ShapePage(title, shape), title);
}

int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app){
        auto window = Window(app);
        auto stack = Stack();

        add_page(stack, "Point", Shape::Point({0, 0}));
        add_page(stack, "Points", Shape::Points({{-0.5, 0.5}, {0.5, 0.5}, {0.5, -0.5}}));

        auto box = Box(Orientation::HORIZONTAL);
        box.push_back(stack);
        box.push_back(StackSidebar(stack));

        window.set_child(box);
        window.present();
    });
    app.run();
}
