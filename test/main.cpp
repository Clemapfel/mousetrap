#include <mousetrap.hpp>

using namespace mousetrap;

int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        static auto window = Window(app);
        static auto transform = TransformBin();

        static auto values = std::vector<double>();
        static auto image_display = ImageDisplay();
        image_display.set_expand(true);

        static auto animation = Animation(image_display, seconds(3));
        animation.set_lower(0.1);
        animation.set_upper(0.9);
        animation.on_tick([](Animation&, double value){
            values.push_back(value);
            transform.reset();
            transform.translate({0,-1 * (value * 200)});
        });
        animation.on_done([](Animation&){
            auto image = Image();
            size_t height = std::min<size_t>(400, values.size());
            image.create(values.size() + 1, height + 1, RGBA(0, 0, 0, 1));

            for (size_t i = 0; i < values.size(); ++i)
                image.set_pixel(i, glm::clamp<int>(round(values.at(i) * height), 0, height), RGBA(0, 1, 0, 1));

            transform.reset();
            image_display.create_from_image(image.as_flipped(false, true));
        });

        auto dropdown = DropDown();
        dropdown.set_expand_vertically(false);
        image_display.set_expand_vertically(true);

        auto add_option = [&](const std::string& label, Animation::TweeningCurve curve){
            dropdown.push_back(Label(label), Label(label), [curve](DropDown&) {
                animation.set_tweening_mode(curve);
                values.clear();
                animation.reset();
                animation.play();
                image_display.create_from_image(Image(1, 1, RGBA(0, 0, 0, 0)));
            });
        };

        #define ADD_OPTION(NAME) \
            add_option(#NAME, Animation::TweeningCurve::NAME); \
            add_option(#NAME + std::string("_REVERSE"), Animation::TweeningCurve::NAME##_REVERSE); \
            add_option(#NAME + std::string("_SIGMOID"), Animation::TweeningCurve::NAME##_SIGMOID); \

        ADD_OPTION(EXPONENTIAL);
        ADD_OPTION(SINE);
        ADD_OPTION(CIRCULAR);
        ADD_OPTION(BACK);
        ADD_OPTION(ELASTIC);
        ADD_OPTION(BOUNCE);

        auto box = Box(Orientation::VERTICAL);
        box.push_back(image_display);
        box.push_back(dropdown);
        transform.set_child(box);
        window.set_child(transform);
        window.present();

    });
    return app.run();
}