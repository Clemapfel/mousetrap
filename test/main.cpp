#include <mousetrap.hpp>

using namespace mousetrap;

#include <thread>
#include <chrono>

void changed() {
    std::cout << "called" << std::endl;
}

int main()
{
    std::ios_base::Init();

    // declare application
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);

        auto overlay = PopupMessageOverlay();
        overlay.set_child(Separator());
        overlay.set_size_request({300, 300});
        auto message = PopupMessage("message");
        message.set_timeout(microseconds(1));
        overlay.show_message(message);

        window.set_child(overlay);
        window.present();
    });
    return app.run();
}
