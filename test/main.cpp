#include <mousetrap.hpp>

using namespace mousetrap;

#include <thread>
#include <chrono>

int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app){
        auto window = Window(app);
        auto dropdown = DropDown();
        dropdown.push_back(Label("test"), Label("test"), [](DropDown&){});
        dropdown.push_back(Label("test"), Label("test"), [](DropDown&){});
        window.set_child(dropdown);

        static auto texture = Texture();
        texture.create_from_file("/home/clem/Downloads/PXL_20230908_092017150.jpg");

        auto canvas = RenderArea();
        auto shape = Shape::Rectangle({-1, 1}, {2, 2});
        shape.set_texture(&texture);
        texture.set_scale_mode(TextureScaleMode::NEAREST);

        auto task = RenderTask(shape);
        canvas.add_render_task(task);

        auto frame = AspectFrame(1.0);
        frame.set_child(canvas);
        frame.set_size_request({150, 150});
        window.set_child(frame);
        window.present();
    });
    app.run();
}

#if FALSE

void changed() {
    std::cout << "called" << std::endl;
}

int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);
        window.set_title("mousetrap");

        static auto area = RenderArea(AntiAliasingQuality::BEST);
        static auto shape = Shape::Rectangle({-0.5, 0.5}, {1, 1});
        shape.set_vertex_color(0, HSVA(0.1, 1, 1, 1));
        shape.set_vertex_color(1, HSVA(0.3, 1, 1, 1));
        shape.set_vertex_color(2, HSVA(0.6, 1, 1, 1));
        shape.set_vertex_color(3, HSVA(0.9, 1, 1, 1));

        area.add_render_task(RenderTask(shape));
        auto frame = AspectFrame(1.0);
        frame.set_child(area);
        frame.set_size_request({150, 150});

        static auto animation = Animation(area, seconds(3));
        animation.set_repeat_count(0);
        animation.on_tick([](Animation&, double value){
            static auto current = 0;
            shape.rotate(degrees(-1 * current), shape.get_centroid());
            shape.rotate(degrees(value * 360), shape.get_centroid());
            current = value * 360;
            area.queue_render();
        });

        area.connect_signal_map([](RenderArea& self){
           animation.play();
        });

        window.set_child(frame);
        window.present();
    });
    return app.run();
}
    #endif
