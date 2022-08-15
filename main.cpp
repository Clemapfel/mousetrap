//
// Created by clem on 7/31/22.
//

#include <gtk/gtk.h>

#include <include/window.hpp>

#include <app/initialize.hpp>
#include <app/palette_view.hpp>

using namespace mousetrap;

int main()
{
    auto main_window = initialize();
    main_window.add_events(GDK_KEY_PRESS_MASK);
    main_window.add_events(GDK_KEY_RELEASE_MASK);

    state::palette_view_instance = new PaletteView();
    state::palette_view_instance->set_sort_mode(PaletteSortMode::BY_VALUE);

    main_window.add(state::palette_view_instance);

    // render loop
    gtk_widget_show_all(main_window.get_native());
    main_window.present();
    gtk_main();

    return 0;
}