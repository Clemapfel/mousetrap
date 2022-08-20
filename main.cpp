//
// Created by clem on 7/31/22.
//

#include <gtk/gtk.h>

#include <include/window.hpp>
#include <include/box.hpp>
#include <include/entry.hpp>

#include <app/initialize.hpp>
#include <app/palette_view.hpp>
#include <app/primary_secondary_color_swapper.hpp>
#include <app/verbose_color_picker.hpp>
#include <app/color_picker.hpp>

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
    state::verbose_color_picker_instance = new VerboseColorPicker();

    auto color_picker = ColorPicker();
    color_picker.update();
    main_window.add(color_picker);

    if (false) {
        auto* tree_store = gtk_tree_store_new(2, G_TYPE_STRING, GDK_TYPE_PIXBUF);

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

        GtkTreeIter pixbuf_col_it;

        auto image = Image();
        image.create(100, 100, RGBA(1, 0, 1, 0.1));
        auto box = Box(GTK_ORIENTATION_HORIZONTAL);
        auto entry = Entry();
        box.add(&entry);

        gtk_tree_store_append(tree_store, &toplevel_it, nullptr);
        gtk_tree_store_set(tree_store, &child_it,
                           1, image.to_pixbuf(), -1);

        auto* tree_view = gtk_tree_view_new();
        gtk_tree_view_set_reorderable(GTK_TREE_VIEW(tree_view), true);

        auto* text_column = gtk_tree_view_column_new();
        gtk_tree_view_column_set_title(text_column, "text");
        auto* text_renderer = gtk_cell_renderer_accel_new();
        gtk_tree_view_column_pack_start(text_column, text_renderer, true);
        gtk_tree_view_column_add_attribute(text_column, text_renderer, "text", 0);

        auto* pixbuf_column = gtk_tree_view_column_new();
        gtk_tree_view_column_set_title(pixbuf_column, "pixbuf");
        auto* pixbuf_renderer = gtk_cell_renderer_pixbuf_new();
        gtk_tree_view_column_pack_start(pixbuf_column, pixbuf_renderer, true);
        gtk_tree_view_column_set_attributes(pixbuf_column, pixbuf_renderer, "pixbuf", 1, nullptr);

        gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), GTK_TREE_MODEL(tree_store));
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), text_column);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), pixbuf_column);

        main_window.add(tree_view);
    }
    gtk_widget_show_all(main_window.get_native());
    main_window.present();
    gtk_main();

    return 0;
}