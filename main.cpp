//
// Created by clem on 7/31/22.
//

#include <gtk/gtk.h>

#include <include/window.hpp>
#include <include/box.hpp>
#include <include/entry.hpp>
#include <include/application.hpp>
#include <include/gl_area.hpp>
#include <include/image_display.hpp>
#include <include/event_controller.hpp>
#include <include/menu_model.hpp>
#include <include/menu_button.hpp>
#include <include/column_view.hpp>
#include <include/image_display.hpp>
#include <include/list_view.hpp>
#include <include/detachable_box.hpp>
#include <include/paned.hpp>
#include <include/dropdown.hpp>
#include <include/button.hpp>
#include <include/check_button.hpp>
#include <include/grid_view.hpp>
#include <include/reorderable_list.hpp>
#include <include/dialog.hpp>

#include <app/global_state.hpp>
#include <app/primary_secondary_color_swapper.hpp>
#include <app/brush_options.hpp>
#include <app/palette_view.hpp>
#include <app/toolbox.hpp>
#include <app/color_picker.hpp>
#include <app/widget_container.hpp>
#include <app/global_state.hpp>
#include <app/preview.hpp>
#include <app/layer_view.hpp>
#include <app/verbose_color_picker.hpp>
#include <app/menubar.hpp>
#include <app/canvas.hpp>
#include <app/app_layout.hpp>

using namespace mousetrap;

void initialize_debug_layers()
{
    state::layer_resolution = {50, 50};

    state::layers.emplace_back(new Layer{"number"});

    for (size_t i = 0; i < 5; ++i)
    {
        state::layers.emplace_back(new Layer{"overlay"});
        state::layers.back()->blend_mode = BlendMode::MULTIPLY;
    }

    auto* preview = new Preview();
    for (auto* layer : state::layers)
    {
        layer->frames.clear();
        layer->frames.resize(10);

        size_t frame_i = 0;
        for (auto& frame: layer->frames)
        {
            frame.image = new Image();
            if (layer->name == "number")
            {
                frame.image->create_from_file(
                        get_resource_path() + "example_animation/0" + std::to_string(frame_i) + ".png");
            }
            else
            {
                frame.image->create(state::layer_resolution.x, state::layer_resolution.y,
                                    RGBA(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX),
                                         0.25));
            }

            frame.texture = new Texture();
            frame.texture->create_from_image(*frame.image);
            frame_i += 1;
        }

        state::n_frames = layer->frames.size();
    }
}

static void activate(GtkApplication* app, void*)
{
    state::shortcut_map = new ShortcutMap();
    state::shortcut_map->load_from_file("/home/clem/Workspace/mousetrap/app/keybindings.conf");

    state::main_window = new Window(GTK_WINDOW(gtk_application_window_new(app)));
    gtk_initialize_opengl(GTK_WINDOW(state::main_window->operator GtkWidget*()));
    state::app->add_window(state::main_window);
    state::main_window->set_show_menubar(true);

    initialize_debug_layers();

    state::primary_secondary_color_swapper = new PrimarySecondaryColorSwapper();
    state::palette_view = new PaletteView();
    state::brush_options = new BrushOptions();
    state::toolbox = new Toolbox();
    state::color_picker = new ColorPicker();
    state::layer_view = new LayerView();
    state::verbose_color_picker = new VerboseColorPicker();
    state::canvas = new Canvas();

    // TODO
    auto* layer_view = state::layer_view->operator Widget*();
    auto* palette_view = state::palette_view->operator Widget*();
    auto* color_swapper = state::primary_secondary_color_swapper->operator Widget*();
    auto* color_picker = state::color_picker->operator Widget*();
    auto* verbose_color_picker = state::verbose_color_picker->operator Widget*();
    auto* canvas = state::canvas->operator Widget*();
    auto* toolbox = state::toolbox->operator Widget*();
    auto* brush_options = state::brush_options->operator Widget*();

    auto* left_column = new Box(GTK_ORIENTATION_VERTICAL);
    auto* center_column = new Box(GTK_ORIENTATION_VERTICAL);
    auto* right_column = new Box(GTK_ORIENTATION_VERTICAL);

    for (auto* column : {left_column, right_column})
        column->set_size_request({state::margin_unit * 2, 0});

    left_column->set_hexpand(false);
    right_column->set_hexpand(false);
    center_column->set_hexpand(true);

    auto* left_center_paned = new Paned(GTK_ORIENTATION_HORIZONTAL);
    auto* center_right_paned = new Paned(GTK_ORIENTATION_HORIZONTAL);

    left_center_paned->set_start_child(left_column);
    left_center_paned->set_end_child(center_right_paned);
    center_right_paned->set_start_child(center_column);
    center_right_paned->set_end_child(right_column);

    left_center_paned->set_start_child_shrinkable(false);
    center_right_paned->set_end_child_shrinkable(false);

    left_center_paned->set_hexpand(false);
    center_right_paned->set_hexpand(false);

    auto add_spacer = [](Box* column)
    {
        auto* spacer = new SeparatorLine();
        spacer->set_size_request({3, 3});
        column->push_back(spacer);
    };

    // LEFT COLUMN

    float color_picker_width = 20 * state::margin_unit;
    float color_swapper_height = 5 * state::margin_unit;

    color_picker->set_size_request({color_picker_width, color_picker_width});
    color_swapper->set_size_request({color_picker_width, color_swapper_height});

    for (auto* w : {color_picker, color_swapper})
        w->set_valign(GTK_ALIGN_END);

    auto* palette_view_spacer_paned = new Paned(GTK_ORIENTATION_HORIZONTAL);
    auto* palette_view_spacer = new SeparatorLine();
    palette_view_spacer->set_hexpand(true);
    palette_view_spacer->set_size_request({state::margin_unit, 0});
    palette_view->set_vexpand(true);

    palette_view_spacer_paned->set_start_child(palette_view);
    palette_view_spacer_paned->set_end_child(palette_view_spacer);
    palette_view_spacer_paned->set_start_child_shrinkable(false);
    palette_view_spacer_paned->set_end_child_shrinkable(false);

    auto* left_column_paned_top = new Box(GTK_ORIENTATION_VERTICAL);
    auto* spacer = new SeparatorLine();
    spacer->set_vexpand(true);

    palette_view_spacer_paned->set_vexpand(true);
    left_column_paned_top->push_back(palette_view_spacer_paned);
    left_column_paned_top->push_back(color_swapper);
    add_spacer(left_column_paned_top);
    left_column_paned_top->push_back(color_picker);

    verbose_color_picker->set_vexpand(false);

    auto* left_column_paned = new Paned(GTK_ORIENTATION_VERTICAL);
    left_column_paned->set_start_child(left_column_paned_top);
    left_column_paned->set_end_child(verbose_color_picker);
    left_column_paned->set_start_child_shrinkable(false);
    left_column_paned->set_start_child_resizable(false);
    left_column_paned->set_end_child_resizable(false);
    left_column_paned->set_has_wide_handle(true);

    //left_column->push_back(palette_view_spacer_paned);
    //add_spacer(left_column);
    left_column->push_back(left_column_paned);

    // RIGHT COLUMN

    brush_options->set_vexpand(false);
    toolbox->set_vexpand(true);
    layer_view->set_vexpand(true);

    auto* right_column_paned = new Paned(GTK_ORIENTATION_VERTICAL);

    right_column_paned->set_start_child(brush_options);
    right_column_paned->set_end_child(layer_view);
    right_column_paned->set_start_child_shrinkable(true);
    right_column_paned->set_end_child_shrinkable(false);
    right_column_paned->set_has_wide_handle(true);

    right_column->push_back(toolbox);
    add_spacer(right_column);
    right_column->push_back(right_column_paned);

    for (auto* w : {toolbox, brush_options})
    {
        w->set_valign(GTK_ALIGN_START);
        w->set_vexpand(false);
    }

    // CENTER COLUMN

    center_column->set_size_request({state::margin_unit * 50, 0});
    center_column->push_back(canvas);

    auto* main = new Box(GTK_ORIENTATION_VERTICAL);
    main->push_back(left_center_paned);

    // TODO

    state::main_window->set_child(main);
    state::main_window->show();
    state::main_window->present();
    state::main_window->set_focusable(true);
    state::main_window->grab_focus();
}

static void startup(GApplication*)
{
    state::global_menu_bar_model = new MenuModel();
    state::setup_global_menu_bar_model();
    state::app->set_menubar(state::global_menu_bar_model);
}

int main()
{
    state::app = new Application();
    state::app->connect_signal("activate", activate);
    state::app->connect_signal("startup", startup);

    return state::app->run();
}

/*
 *  if (false) {
 *    // TODO

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

    auto model = gtk_multi_selection_new(G_LIST_MODEL(tree_store));
    auto column_view = WidgetWrapper<GtkColumnView>(GTK_COLUMN_VIEW(gtk_column_view_new(GTK_SELECTION_MODEL(model))));

    std::vector<GtkColumnViewColumn*> columns;
    auto factory = WidgetWrapper<GtkSignalListItemFactory>(GTK_SIGNAL_LIST_ITEM_FACTORY(gtk_signal_list_item_factory_new()));
    factory.connect_signal("setup", on_setup);

    for (size_t i = 0; i < 10; ++i)
        columns.push_back(gtk_column_view_column_new(std::to_string(i).c_str(), GTK_LIST_ITEM_FACTORY(factory._native)));

    for (auto* col : columns)
        gtk_column_view_append_column(column_view._native, col);

    auto store = gtk_list_store_new(10);

    // TODO
 *
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
 */