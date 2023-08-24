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

        window.get_header_bar().apply_style_class(mousetrap::STYLE_CLASS_BUTTON_FLAT);
        auto style = StyleClass("custom");
        style.set_property("headerbar", "color", "green");

        auto bin = TransformBin();
        //bin.set_child(widget);
        bin.add_css_class("custom");

        auto box = Box(Orientation::VERTICAL);
        box.push_back(bin);

        window.set_child(box);
        window.present();

        window.add_css_class("osd");

        auto chooser = ColorChooser();
        chooser.present();

    });
    return app.run();
}
