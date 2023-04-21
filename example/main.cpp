#include <mousetrap.hpp>

using namespace mousetrap;

struct State {
    Window window;
    Labe label;
}* state = nullptr;

int main()
{
    auto app = Application("mousetrap.example");
    app.connect_signal_activate([](Application* app)
    {
        state = new State{
            Window(*app),
            Label("Hello World")
        }
        state->window.present();
    });

    app.connect_signal_shutdown([](Application* app){
        delete state;
    });

    return app.run();
}