#include <mousetrap.hpp>

using namespace mousetrap;

#include <thread>
#include <chrono>

int main()
{
    // declare application
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);

        /*
        StyleManager::add_css(R"(
            @keyframes example {
              from {background-color: red;}
              to {background-color: yellow;}
            }

            .animated {
                color: blue;
                animation-name: example;
                animation-duration: 4s;
            }

            .custom levelbar>trough>block.low {
                background-color: red;
            }

            .custom levelbar>trough>block.high {
                background-color: blue;
            }

            .custom levelbar>trough>block.full {
                background-color: green;
            }
        )");
         */

        auto widget = window.get_header_bar();

        auto style = StyleClass("custom");
        style.set_property("headerbar>windowhandle>box>widget", "color", "green");

        StyleManager::add_style_class(style);
        std::cout << style.serialize() << std::endl;

        auto bin = TransformBin();
        //bin.set_child(widget);
        bin.add_css_class("custom");

        auto box = Box(Orientation::VERTICAL);
        box.push_back(bin);

        window.set_child(box);
        window.present();
    });
    return app.run();
}
