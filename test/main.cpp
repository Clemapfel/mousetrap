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

        static auto action = Action("test.action", app);
        action.set_function([](Action&){
           std::cout << "called" << std::endl;
        });

        static auto overlay = PopupMessageOverlay();
        auto button = Button();
        button.connect_signal_clicked([](Button&){

            static size_t i = 0;

            auto message = PopupMessage(std::to_string(i) + " title", "button_label");
            message.connect_signal_dismissed([](PopupMessage& message){
               std::cout << "dismissed " <<  message.get_title() << std::endl;
            });
            message.connect_signal_button_clicked([](PopupMessage& message){
               std::cout << "clicked " << message.get_title() << std::endl;
            });
            message.set_button_action(action);
            std::cout << message.get_button_action_id() << std::endl;

            overlay.show_message(message);
        });

        overlay.set_child(button);
        window.set_child(overlay);
        window.present();
    });
    return app.run();
}
