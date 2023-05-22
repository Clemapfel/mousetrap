#include <mousetrap.hpp>
using namespace mousetrap;

int main()
{
    // declare application
    auto app = Application("test.app");

    // initialization
    app.connect_signal_activate([](Application* app)
    {
        // create window
        auto window = Window(*app);

        // create label
        label.set_margin(75);

        // add label to window
        window.set_child(label);

        // show window
        window.present();
    });

    // start main loop
    return app.run();
}