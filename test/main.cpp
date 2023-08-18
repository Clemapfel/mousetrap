#include <mousetrap.hpp>

using namespace mousetrap;



int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);
        window.set_title("");
        static auto child = TransformBin();
        child.set_child(Label(""));
        child.set_expand(true);

        static auto style_class = StyleClass("test_class");
        style_class.set_property(STYLE_PROPERTY_COLOR, "red");
        //app.add_style_class(style_class);
        child.add_css_class(style_class.get_name());

        static auto animation = Animation(child, seconds(1));
        animation.on_tick([&](Animation& self, double value) {

        });
        animation.on_done([](Animation& self){
            child.reset();
        });

        button.connect_signal_clicked([](Button&){
            animation.play();
        });

        auto square = AspectFrame(1.0);
        square.set_child(child);
        window.set_child(square);
        window.present();

    });
    return app.run();
}