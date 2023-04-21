#include <mousetrap.hpp>

using namespace mousetrap;

struct State {
    Window window;
    Label label;
}* state = nullptr;

int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application* app)
    {
        state = new State{
            Window(*app),
            Label("Hello World")
        };
        state->window.set_child(state->label);
        state->window.present();
    });

    app.connect_signal_shutdown([](Application* app){
        delete state;
    });

    return app.run();
}