#include <mousetrap.hpp>

using namespace mousetrap;

#include <thread>
#include <chrono>

int main()
{
    mousetrap::FORCE_GL_DISABLED = true;

    // declare application
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);

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

        auto child = LevelBar(0, 1);
        auto scale = Scale(0, 1, 0.1);

        auto bin = TransformBin();
        bin.set_child(child);
        bin.add_css_class("custom");

        auto box = Box(Orientation::VERTICAL);
        box.push_back(bin);
        box.push_back(scale);

        child.set_expand_vertically(true);
        scale.set_expand_vertically(false);

        window.set_child(box);
        window.present();
    });
    return app.run();
}
