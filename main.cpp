//
// Created by clem on 7/31/22.
//

#include <gtk/gtk.h>

#include <include/window.hpp>

#include <app/initialize.hpp>
#include <app/palette_view.hpp>
#include <app/primary_secondary_color_swapper.hpp>

using namespace mousetrap;

int main()
{
    auto main_window = initialize();
    main_window.add_events(
        GDK_KEY_PRESS_MASK,
        GDK_KEY_RELEASE_MASK,
        GDK_BUTTON_PRESS_MASK,
        GDK_BUTTON_RELEASE_MASK
    );

    state::palette_view_instance = new PaletteView();
    state::palette_view_instance->set_sort_mode(PaletteSortMode::BY_VALUE);

    state::primary_secondary_color_swapper_instance = new PrimarySecondaryColorSwapper();

    main_window.add(state::primary_secondary_color_swapper_instance);

    // render loop
    gtk_widget_show_all(main_window.get_native());
    main_window.present();
    gtk_main();

    return 0;
}