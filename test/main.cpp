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

        StyleManager::add_css(R"(
        @keyframes spin {
          to {
            transform: rotate(1turn);
          }
        }
        @keyframes glow {
            from {
                box-shadow: 0px 0px 0px gold;
            }
            to {
                box-shadow: 0px 0px 50px gold;
            }
        }
        )");

        auto style = StyleClass("custom");
        auto target = "widget";
        style.set_property(target, STYLE_PROPERTY_ANIMATION_NAME, "spin");
        style.set_property(target, STYLE_PROPERTY_ANIMATION_DURATION, "1s");
        style.set_property(target, STYLE_PROPERTY_ANIMATION_ITERATION_COUNT, "infinite");

        style.set_property("separator", STYLE_PROPERTY_ANIMATION_NAME, "glow");
        style.set_property("separator", STYLE_PROPERTY_ANIMATION_DURATION, "5s");
        style.set_property("separator", STYLE_PROPERTY_ANIMATION_TIMING_FUNCTION, "ease-in-out");
        style.set_property("separator", STYLE_PROPERTY_ANIMATION_ITERATION_COUNT, "infinite");
        style.set_property("separator", STYLE_PROPERTY_BORDER_RADIUS, "100%");
        style.set_property("separator", STYLE_PROPERTY_BOX_SHADOW, "0px 0px 20px gold");
        StyleManager::add_style_class(style);

        window.get_header_bar().set_layout(":close");
        window.set_title("how");
        window.add_css_class(style.get_name());
        window.add_css_class(STYLE_CLASS_CARD.get_name());

        auto widget = Button();
        auto child = Separator();
        widget.set_child(child);

        auto bin = TransformBin();
        widget.set_margin(10);
        bin.set_child(widget);

        window.set_child(bin);
        window.present();
    });
    return app.run();
}
