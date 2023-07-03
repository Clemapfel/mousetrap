//
// Created by clem on 4/12/23.
//

#include <mousetrap/application.hpp>
#include <mousetrap/window.hpp>
#include <mousetrap/label.hpp>
#include <mousetrap/motion_event_controller.hpp>
#include <mousetrap/box.hpp>
#include <mousetrap/drop_down.hpp>
#include <mousetrap/button.hpp>
#include <mousetrap/action.hpp>
#include <mousetrap/popover_menu.hpp>
#include <mousetrap/popover_button.hpp>
#include <mousetrap/list_view.hpp>

#include <mousetrap.hpp>

using namespace mousetrap;


int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);

        auto* list_row = gtk_list_box_new();
        gtk_list_box_append(GTK_LIST_BOX(list_row), gtk_label_new("Test 01"));
        gtk_list_box_append(GTK_LIST_BOX(list_row), gtk_button_new_with_label("Button 02"));
        gtk_list_box_append(GTK_LIST_BOX(list_row), gtk_entry_new());

        //gtk_window_set_child(GTK_WINDOW(window.operator NativeWidget()), list_row);

        auto list_view = ListView(Orientation::VERTICAL);

        list_view.push_back(Label("Test 01"));
        // list_view.push_back(Button());
        // list_view.push_back(Entry());

        // auto box = Box(Orientation::HORIZONTAL);
        // gtk_box_append(GTK_BOX(box.operator NativeWidget()), list_row);
        // box.push_back(list_view);

        window.set_child(list_view);
        window.present();
    });

    return app.run();
}
