#include <mousetrap.hpp>

using namespace mousetrap;

int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);
        static auto child = LevelBar(0, 1);
        auto scale = Scale(0, 1, 0.01);
        scale.connect_signal_value_changed([](Scale& self){
            child.set_value(self.get_value());
        });
        child.set_value(scale.get_value());

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