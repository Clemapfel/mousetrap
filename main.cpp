//
// Created by clem on 7/31/22.
//

#include <gtk/gtk.h>

#include <mousetrap.hpp>
#include <app/add_shortcut_action.hpp>
#include <app/algorithms.hpp>
#include <app/animation_preview.hpp>
#include <app/app_component.hpp>
#include <app/brush.hpp>
#include <app/brush_options.hpp>
#include <app/bubble_log_area.hpp>
#include <app/canvas.hpp>
#include <app/color_picker.hpp>
#include <app/color_preview.hpp>
#include <app/color_swapper.hpp>
#include <app/color_transform.hpp>
#include <app/file_chooser_dialog.hpp>
#include <app/frame_view.hpp>
#include <app/project_state.hpp>
#include <app/image_transform.hpp>
#include <app/layer.hpp>
#include <app/layer_view.hpp>
#include <app/menubar.hpp>
#include <app/palette.hpp>
#include <app/palette_view.hpp>
#include <app/save_file.hpp>
#include <app/selection.hpp>
#include <app/config_files.hpp>
#include <app/shortcut_information.hpp>
#include <app/toolbox.hpp>
#include <app/tools.hpp>
#include <app/tooltip.hpp>
#include <app/verbose_color_picker.hpp>
#include <app/config_files.hpp>

using namespace mousetrap;

static void activate(GtkApplication* app, void*)
{
    state::settings_file = new KeyFile(get_resource_path() + "settings.ini");
    state::keybindings_file = new KeyFile(get_resource_path() + "keybindings.ini");
    state::tooltips_file = new KeyFile(get_resource_path() + "tooltips.ini");

    state::main_window = new Window(GTK_WINDOW(gtk_application_window_new(app)));
    gtk_initialize_opengl(GTK_WINDOW(state::main_window->operator GtkWidget*()));
    state::app->add_window(state::main_window);
    state::main_window->set_show_menubar(true);

    state::shortcut_controller = new ShortcutController(state::app);
    state::main_window->add_controller(state::shortcut_controller);

    active_state = project_states.emplace_back(new ProjectState({75, 50}));

    state::color_swapper = new ColorSwapper();
    state::palette_view = new PaletteView();
    state::brush_options = new BrushOptions();
    state::toolbox = new Toolbox();
    state::color_picker = new ColorPicker();
    state::layer_view = new LayerView();
    state::frame_view = new FrameView();
    state::verbose_color_picker = new VerboseColorPicker();
    state::canvas = new Canvas();
    state::color_preview = new ColorPreview();
    state::bubble_log = new BubbleLogArea();
    state::animation_preview = new AnimationPreview();

    auto* layer_view = state::layer_view->operator Widget*();
    auto* palette_view = state::palette_view->operator Widget*();
    auto* color_swapper = state::color_swapper->operator Widget*();
    auto* color_picker = state::color_picker->operator Widget*();
    auto* verbose_color_picker = state::verbose_color_picker->operator Widget*();
    auto* canvas = state::canvas->operator Widget*();
    auto* toolbox = state::toolbox->operator Widget*();
    auto* brush_options = state::brush_options->operator Widget*();
    auto* color_preview = state::color_preview->operator Widget*();
    auto* bubble_log = state::bubble_log->operator Widget*();
    auto* frame_view = state::frame_view->operator Widget*();
    auto* animation_preview = state::animation_preview->operator Widget*();

    auto* color_picker_window = new Window();
    color_picker_window->set_child(color_picker);
    color_picker_window->set_titlebar_layout("title:close");
    color_picker_window->set_modal(false);
    color_picker_window->set_title("HSV Color Picker");
    color_picker_window->set_transient_for(state::main_window);

    auto* show_color_picker_click_ec = new ClickEventController();
    static auto show_color_picker = [](ClickEventController*, size_t n, double, double, Window* window)
    {
        if (n == 2)
        {
            window->show();
            window->present();
        }
    };

    show_color_picker_click_ec->connect_signal_click_pressed(show_color_picker, color_picker_window);
    color_preview->add_controller(show_color_picker_click_ec);

    float color_picker_width = 25 * state::margin_unit;
    float color_swapper_height = 8 * state::margin_unit;

    color_picker->set_size_request(Vector2f(color_picker_width));
    color_swapper->set_size_request({color_picker_width, color_swapper_height});
    color_preview->set_size_request({color_picker_width + 2 * state::margin_unit, color_swapper_height * 0.5});
    color_preview->set_cursor(GtkCursorType::POINTER);

    bubble_log->set_margin(2 * state::margin_unit);
    bubble_log->set_align(GTK_ALIGN_END);
    bubble_log->set_size_request({100, 100});

    // LAYOUT

    auto left_column_paned_top = Box(GTK_ORIENTATION_VERTICAL);
    left_column_paned_top.push_back(palette_view);

    auto left_column_paned_bottom = Box(GTK_ORIENTATION_VERTICAL);

    auto color_preview_frame = Paned(GTK_ORIENTATION_VERTICAL); // for handlebar below color_preview
    color_preview_frame.set_start_child_shrinkable(false);
    color_preview_frame.set_end_child_shrinkable(false);
    color_preview_frame.set_start_child_resizable(false);
    color_preview_frame.set_end_child_resizable(false);

    auto preview_swapper_box = Box(GTK_ORIENTATION_VERTICAL);
    preview_swapper_box.push_back(color_swapper);
    preview_swapper_box.push_back(color_preview);

    color_preview_frame.set_start_child(&preview_swapper_box);

    auto color_preview_frame_buffer = SeparatorLine();
    color_preview_frame_buffer.set_expand(false);
    color_preview_frame_buffer.set_size_request({1, 1});
    color_preview_frame_buffer.set_opacity(0);
    color_preview_frame.set_end_child(&color_preview_frame_buffer);

    left_column_paned_bottom.push_back(&color_preview_frame);
    left_column_paned_bottom.push_back(verbose_color_picker);

    auto left_column_paned = Paned(GTK_ORIENTATION_VERTICAL);
    left_column_paned.set_start_child(&left_column_paned_top);
    left_column_paned.set_end_child(&left_column_paned_bottom);

    palette_view->set_vexpand(true);
    color_preview->set_vexpand(false);
    verbose_color_picker->set_vexpand(true);
    color_preview->set_margin(state::margin_unit);
    verbose_color_picker->set_margin(state::margin_unit);

    auto center_column = Box(GTK_ORIENTATION_VERTICAL);
    center_column.push_back(toolbox);
    center_column.push_back(canvas);

    toolbox->set_expand(false);
    canvas->set_expand(true);

    auto left_and_center_column_paned = Paned(GTK_ORIENTATION_HORIZONTAL);
    left_and_center_column_paned.set_start_child(&left_column_paned);
    left_and_center_column_paned.set_end_child(&center_column);

    auto left_and_center_and_frame_view_paned = Paned(GTK_ORIENTATION_VERTICAL);
    left_and_center_and_frame_view_paned.set_start_child(&left_and_center_column_paned);
    left_and_center_and_frame_view_paned.set_end_child(frame_view);
    left_and_center_and_frame_view_paned.set_start_child_shrinkable(false);

    auto right_column_paned_top = Paned(GTK_ORIENTATION_VERTICAL);
    right_column_paned_top.set_start_child(animation_preview);
    right_column_paned_top.set_end_child(brush_options);

    auto right_column_paned = Paned(GTK_ORIENTATION_VERTICAL);
    right_column_paned.set_start_child(&right_column_paned_top);
    right_column_paned.set_end_child(layer_view);

    auto main_paned = Paned(GTK_ORIENTATION_HORIZONTAL);
    main_paned.set_start_child_shrinkable(false);
    main_paned.set_start_child(&left_and_center_and_frame_view_paned);
    main_paned.set_end_child(&right_column_paned);

    auto main = Box(GTK_ORIENTATION_VERTICAL);
    auto main_spacer_top = SeparatorLine();
    main_spacer_top.set_size_request({0, state::margin_unit / 5});
    main.push_back(&main_spacer_top);
    main.push_back(&main_paned);

    left_column_paned.set_end_child_shrinkable(false);
    left_column_paned.set_position(10e6); // as low as possible
    left_and_center_column_paned.set_end_child_shrinkable(false);
    main_paned.set_start_child_shrinkable(false);

    right_column_paned_top.set_start_child_shrinkable(false);
    right_column_paned_top.set_end_child_shrinkable(false);
    right_column_paned.set_start_child_shrinkable(false);
    right_column_paned.set_end_child_shrinkable(false);
    right_column_paned.set_position(10e6);

    auto bubble_log_overlay = Overlay();
    bubble_log_overlay.set_child(&main);
    bubble_log_overlay.add_overlay(bubble_log);

    // MAIN

    state::main_window->set_child(&bubble_log_overlay);
    state::main_window->show();
    state::main_window->present();
    state::main_window->set_focusable(true);
    state::main_window->grab_focus();
    state::main_window->connect_signal_close([](Window* window, nullptr_t) -> bool {
        return false;
    }, nullptr);

    validate_keybindings_file(state::keybindings_file);
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