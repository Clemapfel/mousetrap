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
#include <app/open_uri.hpp>
#include <app/scale_canvas_dialog.hpp>
#include <app/resize_canvas_dialog.hpp>
#include <app/canvas_export.hpp>

using namespace mousetrap;

static void activate(GtkApplication* app, void*)
{
    state::initialize_config_files();

    state::main_window = new Window(GTK_WINDOW(gtk_application_window_new(app)));
    gtk_initialize_opengl(GTK_WINDOW(state::main_window->operator GtkWidget*()));
    state::app->add_window(state::main_window);
    state::main_window->set_show_menubar(true);

    state::shortcut_controller = new ShortcutController(state::app);
    state::main_window->add_controller(state::shortcut_controller);

    active_state = project_states.emplace_back(new ProjectState({75, 50}));

    /*
    state::color_picker = new ColorPicker();
    state::canvas = new Canvas();
    */
    state::frame_view = new FrameView();
    state::brush_options = new BrushOptions();
    state::color_preview = new ColorPreview();
    state::verbose_color_picker = new VerboseColorPicker();
    state::color_swapper = new ColorSwapper();
    state::bubble_log = new BubbleLogArea();
    state::palette_view = new PaletteView();
    state::animation_preview = new AnimationPreview();
    state::layer_view = new LayerView();
    state::toolbox = new Toolbox();
    state::scale_canvas_dialog = new ScaleCanvasDialog();
    state::resize_canvas_dialog = new ResizeCanvasDialog();
    state::canvas_export = new CanvasExport();

    Widget* layer_view = state::layer_view->operator Widget*();
    Widget* palette_view = state::palette_view->operator Widget*();
    Widget* color_swapper = state::color_swapper->operator Widget*();
    Widget* color_picker = new SeparatorLine(); //state::color_picker->operator Widget*();
    Widget* verbose_color_picker = state::verbose_color_picker->operator Widget*();
    Widget* canvas = new SeparatorLine(); //state::canvas->operator Widget*();
    Widget* toolbox = state::toolbox->operator Widget*();
    Widget* brush_options = state::brush_options->operator Widget*();
    Widget* color_preview = state::color_preview->operator Widget*();
    Widget* bubble_log = state::bubble_log->operator Widget*();
    Widget* frame_view = state::frame_view->operator Widget*();
    Widget* animation_preview = state::animation_preview->operator Widget*();
    Widget* resize_canvas_dialog = state::resize_canvas_dialog->operator Widget*();
    Widget* canvas_export = state::canvas_export->operator Widget*();

    toolbox->set_vexpand(false);

    canvas->set_size_request({500, 0});

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

    right_column_paned.set_position(-10e6); // fully extend layer view

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

    // c.f. canvas_export.hpp @note
    bubble_log_overlay.add_overlay(canvas_export);
    canvas_export->set_halign(GTK_ALIGN_START);
    canvas_export->set_valign(GTK_ALIGN_START);
    canvas_export->set_can_respond_to_input(false);
    canvas_export->set_opacity(1);

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
