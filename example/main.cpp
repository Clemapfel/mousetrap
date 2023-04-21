#include <mousetrap.hpp>

using namespace mousetrap;

int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application* app)
    {
        auto window = Window(*app);

        auto label = Label("Hello World");
        label.set_margin(75);

        window.set_child(label);
        window.present();
    });

    return app.run();
}