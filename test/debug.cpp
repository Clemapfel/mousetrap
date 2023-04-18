#include <mousetrap.hpp>
using namespace mousetrap;

int main()
{
    auto app = Application("mousetrap.test.debug");

    app.connect_signal_activate([](Application* app)
    {
        auto window = Window(*app);
        window.present();
    });

    return app.run();
}