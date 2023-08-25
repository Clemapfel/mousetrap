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
        auto style = StyleClass("custom");
        auto target = "notebook>header.bottom>tabs>tab";
        style.set_property(target, STYLE_PROPERTY_COLOR, "lightgreen");
        style.set_property(target, STYLE_PROPERTY_BACKGROUND_COLOR, "green");
        StyleManager::add_style_class(style);

        auto widget = Notebook();
        widget.push_back(Button(), Label("01"));
        widget.push_back(Button(), Label("02"));
        widget.push_back(Button(), Label("03"));

        auto bin = TransformBin();
        widget.add_css_class(style.get_name());
        widget.set_expand(true);
        widget.set_margin(10);
        bin.set_child(widget);
        bin.add_css_class("custom");

        auto box = Box(Orientation::VERTICAL);
        box.push_back(bin);

        window.set_child(box);
        window.present();

        window.add_css_class("osd");
    });
    return app.run();
}
