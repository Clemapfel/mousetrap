#include <mousetrap.hpp>

using namespace mousetrap;



int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);
        static auto child = TransformBin();
        child.set_child(Label("TEST"));

        static auto style_class = StyleClass("test_class");
        app.add_style_class(style_class);
        child.add_css_class(style_class.get_name());

        static auto animation = Animation(child, seconds(1));
        animation.on_tick([&](Animation& self, double value) {
            child.reset();
            child.rotate(degrees(value * 360));
            child.translate(Vector2f(value * 10, value * 10));
        });
        animation.on_done([](Animation& self){
            child.reset();
        });

        auto button = Button();
        button.connect_signal_clicked([](Button&){
            animation.play();
        });

        auto box = Box(Orientation::HORIZONTAL);
        box.push_back(child);
        box.push_back(button);

        window.set_child(box);
        window.present();

    });
    return app.run();
}