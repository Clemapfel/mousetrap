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

    auto* tree_store = gtk_tree_store_new(1, G_TYPE_STRING);

    GtkTreeIter toplevel_it, child_it;

    gtk_tree_store_append(tree_store, &toplevel_it, nullptr);
    gtk_tree_store_set(tree_store, &toplevel_it,
       0, "test_top_01", -1);

    gtk_tree_store_append(tree_store, &child_it, &toplevel_it);
    gtk_tree_store_set(tree_store, &child_it,
       0, "test_child_01_01", -1);

    gtk_tree_store_append(tree_store, &child_it, &toplevel_it);
    gtk_tree_store_set(tree_store, &child_it,
       0, "test_child_01_02", -1);

    gtk_tree_store_append(tree_store, &child_it, &toplevel_it);
    gtk_tree_store_set(tree_store, &child_it,
       0, "test_child_01_03", -1);

    gtk_tree_store_append(tree_store, &toplevel_it, nullptr);
    gtk_tree_store_set(tree_store, &toplevel_it,
       0, "test_top_02", -1);

    gtk_tree_store_append(tree_store, &child_it, &toplevel_it);
    gtk_tree_store_set(tree_store, &child_it,
       0, "test_child_02_01", -1);

    gtk_tree_store_append(tree_store, &child_it, &toplevel_it);
    gtk_tree_store_set(tree_store, &child_it,
       0, "test_child_02_02", -1);

    gtk_tree_store_append(tree_store, &child_it, &toplevel_it);
    gtk_tree_store_set(tree_store, &child_it,
       0, "test_child_02_03", -1);

    auto* tree_view = gtk_tree_view_new();

    auto* column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(column, "title");
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    auto* renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, true);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 0);

    gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), GTK_TREE_MODEL(tree_store));

    main_window.add(tree_view);
    gtk_widget_show_all(main_window.get_native());
    main_window.present();
    gtk_main();

    return 0;
}