#include <app/menubar.hpp>
#include <app/bubble_log_area.hpp>
#include <app/brush_options.hpp>
#include <app/resize_canvas_dialog.hpp>
#include <app/scale_canvas_dialog.hpp>

namespace mousetrap
{
    static void temp(GAction* action, GVariant* variant)
    {
        std::cout << "called state: " << g_variant_get_boolean(variant) << std::endl;
    }

    void setup_global_menu_bar_model()
    {
        state::global_menu_bar_model = new MenuModel();
        auto& main = *state::global_menu_bar_model;
        auto* app = state::app;

        using namespace state::actions;

        // FILE

        auto file_submenu = MenuModel();

        auto file_submenu_save_section = MenuModel();
        file_submenu_save_section.add_action("New", save_file_new_state.get_id());
        file_submenu_save_section.add_action("Open...", save_file_open_load_dialog.get_id());
        file_submenu_save_section.add_action("Save", save_file_save_state_to_file.get_id());
        file_submenu_save_section.add_action("Save As...", save_file_open_save_dialog.get_id());
        file_submenu_save_section.add_action("Restore from Backup...", save_file_open_restore_from_backup_dialog.get_id());
        file_submenu.add_section("Load / Save", &file_submenu_save_section);

        auto file_submenu_import_section = MenuModel();
        file_submenu_import_section.add_action("Import...", save_file_import_from_image.get_id());
        file_submenu_import_section.add_action("Export As...", save_file_export_as_image.get_id());
        file_submenu_import_section.add_action("Export As Spritesheet...", save_file_export_as_spritesheet.get_id());
        file_submenu_import_section.add_action("Export Spritesheet Metadata...", save_file_export_metadata.get_id());
        file_submenu.add_section("Import / Export", &file_submenu_import_section);

        auto file_submenu_close_section = MenuModel();
        file_submenu_close_section.add_action("Exit", save_file_safe_exit.get_id());

        // SELECTION

        auto selection_submenu = MenuModel();
        auto selection_section = MenuModel();

        selection_section.add_action("Invert", selection_invert.get_id());
        selection_section.add_action("Select All", selection_select_all.get_id());
        selection_section.add_action("Select Color...", selection_open_select_color_dialog.get_id());
        selection_submenu.add_section("Selection", &selection_section);

        // IMAGE

        auto image_submenu = MenuModel();

        auto image_submenu_size_section = MenuModel();
        image_submenu_size_section.add_action("Resize Canvas...", resize_canvas_dialog_open.get_id());
        image_submenu_size_section.add_action("Scale Image...", scale_canvas_dialog_open.get_id());
        image_submenu.add_section("Size", &image_submenu_size_section);

        auto image_submenu_transform_all_section = MenuModel();
        image_submenu_transform_all_section.add_action("Flip Horizontally", image_transform_flip_horizontally.get_id());
        image_submenu_transform_all_section.add_action("Flip Vertically", image_transform_flip_vertically.get_id());
        image_submenu_transform_all_section.add_action("Rotate +90°", image_transform_rotate_clockwise.get_id());
        image_submenu_transform_all_section.add_action("Rotate -90°", image_transform_rotate_counterclockwise.get_id());
        image_submenu.add_section("Transform", &image_submenu_transform_all_section);

        auto image_submenu_transform_current_section = MenuModel();
        image_submenu_transform_current_section.add_action("Flip Layer Horizontally", image_transform_flip_current_layer_horizontally.get_id());
        image_submenu_transform_current_section.add_action("Flip Layer Vertically", image_transform_flip_current_layer_vertically.get_id());
        image_submenu.add_section("Transform Layer", &image_submenu_transform_current_section);

        // COLORS

        auto colors_submenu = MenuModel();

        auto color_submenu_transform_section = MenuModel();
        color_submenu_transform_section.add_action("Invert", color_transform_invert.get_id());
        color_submenu_transform_section.add_action("To Grayscale", color_transform_to_grayscale.get_id());
        color_submenu_transform_section.add_action("Hue...", color_transform_open_hsv_shift_dialog.get_id());
        color_submenu_transform_section.add_action("Saturation...", color_transform_open_hsv_shift_dialog.get_id());
        color_submenu_transform_section.add_action("Value...", color_transform_open_hsv_shift_dialog.get_id());
        color_submenu_transform_section.add_action("Replace...", color_transform_open_color_replace_dialog.get_id());
        color_submenu_transform_section.add_action("Palette Shift...", color_transform_open_palette_shift_dialog.get_id());
        colors_submenu.add_section("Transform", &color_submenu_transform_section);

        auto palette_editing_section = MenuModel();
        palette_editing_section.add_stateful_action("Toggle Palette Locked", palette_view_toggle_palette_locked.get_id(), false);
        colors_submenu.add_section("Palette Editing", &palette_editing_section);


        auto color_palette_section = MenuModel();
        color_palette_section.add_action("Load...", palette_view_load.get_id());
        color_palette_section.add_action("Load Default", palette_view_load_default.get_id());
        color_palette_section.add_action("Save", palette_view_save.get_id());
        color_palette_section.add_action("Save As...", palette_view_save_as.get_id());
        color_palette_section.add_action("Save As Default", palette_view_save_as_default.get_id());
        colors_submenu.add_section("Palette Load / Save", &color_palette_section);


        // LAYERS

        auto layer_submenu = MenuModel();

        auto layer_submenu_create_section = MenuModel();
        layer_submenu_create_section.add_action("New Layer", layer_view_layer_new_above_current.get_id());
        layer_submenu_create_section.add_action("New Layer Below", layer_view_layer_new_below_current.get_id());
        layer_submenu_create_section.add_action("New Layer From Visible", layer_view_layer_create_from_visible.get_id());
        layer_submenu_create_section.add_action("Duplicate Layer", layer_view_layer_duplicate.get_id());
        layer_submenu_create_section.add_action("Delete Layer", layer_view_layer_delete.get_id());
        layer_submenu.add_section("Create / Delete", &layer_submenu_create_section);

        auto layer_submenu_merge_section = MenuModel();
        layer_submenu_merge_section.add_action("Merge Down", layer_view_layer_merge_down.get_id());
        layer_submenu_merge_section.add_action("Flatten All", layer_view_layer_flatten_all.get_id());
        layer_submenu.add_section("Merge", &layer_submenu_merge_section);

        auto layer_move_section = MenuModel();
        layer_move_section.add_action("Move Layer Down", layer_view_layer_move_down.get_id());
        layer_move_section.add_action("Move Layer Up", layer_view_layer_move_up.get_id());
        layer_submenu.add_section("Layer Position", &layer_move_section);

        auto layer_property_section = MenuModel();
        layer_property_section.add_stateful_action("Toggle Layer Visible", layer_view_toggle_layer_visible.get_id(), true);
        layer_property_section.add_stateful_action("Toggle Layer Locked", layer_view_toggle_layer_locked.get_id(), true);
        layer_submenu.add_section("Layer Properties", &layer_property_section);

        auto layer_show_section = MenuModel();
        layer_show_section.add_action("Hide All Other Layers", layer_view_hide_all_other_layers.get_id());
        layer_show_section.add_action("Show All Other Layers", layer_view_show_all_other_layers.get_id());
        layer_submenu.add_section("Hide / Show", &layer_show_section);

        // FRAMES

        auto frame_submenu = MenuModel();

        auto frame_create_section = MenuModel();
        frame_create_section.add_action("New Frame", frame_view_frame_new_right_of_current.get_id());
        frame_create_section.add_action("New Frame Before", frame_view_frame_new_left_of_current.get_id());
        frame_create_section.add_action("Delete Frame", frame_view_frame_delete.get_id());
        frame_submenu.add_section("Create / Delete", &frame_create_section);

        auto frame_move_section = MenuModel();
        frame_move_section.add_action("Move Frame Left", frame_view_frame_move_left.get_id());
        frame_move_section.add_action("Move Frame Right", frame_view_frame_move_right.get_id());
        frame_submenu.add_section("Frame Position", &frame_move_section);

        auto frame_navigation_section = MenuModel();
        frame_navigation_section.add_action("Play / Pause Playback", frame_view_toggle_playback_active.get_id());
        frame_navigation_section.add_action("Select Previous", frame_view_go_to_previous_frame.get_id());
        frame_navigation_section.add_action("Select Next", frame_view_go_to_next_frame.get_id());
        frame_navigation_section.add_action("Jump to Start", frame_view_jump_to_start.get_id());
        frame_navigation_section.add_action("Jump to End", frame_view_jump_to_end.get_id());
        frame_submenu.add_section("Playback / Navigation", &frame_navigation_section);

        auto copy_to_cell_section = MenuModel();
        copy_to_cell_section.add_action("Copy to Cell Above", frame_view_copy_to_cell_above.get_id());
        copy_to_cell_section.add_action("Copy to Cell Below", frame_view_copy_to_cell_below.get_id());
        copy_to_cell_section.add_action("Copy to Cell Before", frame_view_copy_to_cell_before.get_id());
        copy_to_cell_section.add_action("Copy to Cell After", frame_view_copy_to_cell_after.get_id());
        frame_submenu.add_section("Copy Cells", &copy_to_cell_section);

        auto swap_with_cell_section = MenuModel();
        swap_with_cell_section.add_action("Swap with Cell Above", frame_view_swap_with_cell_above.get_id());
        swap_with_cell_section.add_action("Swap with Cell Below", frame_view_swap_with_cell_below.get_id());
        swap_with_cell_section.add_action("Swap with Cell Before", frame_view_swap_with_cell_before.get_id());
        swap_with_cell_section.add_action("Swap with Cell After", frame_view_swap_with_cell_after.get_id());
        frame_submenu.add_section("Swap Cells", &swap_with_cell_section);


        auto frame_other_section = MenuModel();
        frame_other_section.add_stateful_action("Toggle Keyframe", frame_view_toggle_current_frame_is_keyframe.get_id(), false);
        frame_other_section.add_stateful_action("Toggle Onionskin Visible", frame_view_toggle_onionskin_visible.get_id(), false);
        frame_submenu.add_section("Other", &frame_other_section);

        // CANVAS

        auto canvas_submenu = MenuModel();

        auto canvas_grid_section = MenuModel();
        canvas_grid_section.add_action("Hide / Show Grid", canvas_toggle_grid_visible.get_id());
        canvas_grid_section.add_action("Grid Color...", canvas_open_grid_color_settings.get_id());
        canvas_submenu.add_section("Grid", &canvas_grid_section);

        auto canvas_brush_section = MenuModel();
        canvas_brush_section.add_action("Hide / Show Brush Outline", canvas_toggle_brush_outline_visible.get_id());
        canvas_submenu.add_section("Brush", &canvas_brush_section);

        auto canvas_mirror_section = MenuModel();
        canvas_mirror_section.add_action("Enable / Disable Horizontal Symmetry", canvas_toggle_brush_outline_visible.get_id());
        canvas_mirror_section.add_action("Enable / Disable Vertical Symmetry", canvas_toggle_brush_outline_visible.get_id());
        canvas_submenu.add_section("Symmetry", &canvas_mirror_section);

        auto canvas_perspective_section = MenuModel();
        canvas_perspective_section.add_action("Reset View", canvas_reset_transform.get_id());
        canvas_submenu.add_section("View", &canvas_perspective_section);

        // BRUSHES

        auto brushes_submenu = MenuModel();
        auto brush_add_section = MenuModel();
        brush_add_section.add_action("Add Brush...", brush_options_add_brush.get_id());
        brush_add_section.add_action("Remove Current Brush", brush_options_remove_brush.get_id());
        brushes_submenu.add_section("Add / Remove", &brush_add_section);

        auto brush_default_section = MenuModel();
        brush_default_section.add_action("Load Default Brushes", brush_options_load_default_brushes.get_id());
        brush_default_section.add_action("Manage Default Brushes...", brush_options_open_default_brush_directory.get_id());
        brushes_submenu.add_section("Defaults", &brush_default_section);

        // SETTINGS

        auto settings_submenu = MenuModel();

        auto settings_other_section = MenuModel();
        settings_other_section.add_action("Animation...", menu_open_animation_settings.get_id());
        settings_other_section.add_action("Colors...", menu_open_color_settings.get_id());
        settings_submenu.add_section("Project", &settings_other_section);

        auto settings_input_section = MenuModel();
        settings_input_section.add_action("Mouse...", menu_open_cursor_settings.get_id());
        settings_input_section.add_action("Tablet...", menu_open_cursor_settings.get_id());
        settings_input_section.add_action("Keybindings...", menu_open_keybinding_settings.get_id());
        settings_submenu.add_section("User Input", &settings_input_section);

        auto settings_system_section = MenuModel();
        settings_system_section.add_action("Backups...", menu_open_backup_settings.get_id());
        settings_system_section.add_action("Developer Settings...", menu_open_settings_ini_file.get_id());
        settings_submenu.add_section("System", &settings_system_section);

        // OTHER

        auto other_submenu = MenuModel();

        auto undo_section = MenuModel();
        undo_section.add_action("Undo", state_undo.get_id());
        undo_section.add_action("Redo", state_redo.get_id());
        other_submenu.add_section("Edit History", &undo_section);

        auto online_section = MenuModel();
        online_section.add_action("Manual...", menu_open_manual_link.get_id());
        online_section.add_action("GitHub...", menu_open_github_link.get_id());
        online_section.add_action("Donate...", menu_open_donate_link.get_id());
        other_submenu.add_section("Online", &online_section);

        auto troubleshooting_section = MenuModel();
        troubleshooting_section.add_action("About...", menu_open_about_dialog.get_id());
        troubleshooting_section.add_action("Logs...", menu_open_log_folder.get_id());
        other_submenu.add_section("Troubleshooting", &troubleshooting_section);

        // TODO

        static auto* action = new Action("stateful");
        action->set_stateful_function([](bool current) {
            std::cout << current << " -> " << not current << std::endl;
            return not current;
        });
        state::app->add_action(*action);
        other_submenu.add_action("Stateful", action->get_id());

        /*
        auto* _native = other_submenu.operator GMenuModel*();
        auto* item = g_menu_item_new("Stateful", ("app." + action->get_id()).c_str());
        g_menu_item_set_attribute_value(item, "use-markup", g_variant_new_string(true ? "yes" : "no"));
        //g_menu_item_set_attribute_value(item, G_MENU_ATTRIBUTE_TARGET, g_variant_new_boolean(true));
        g_menu_append_item(G_MENU(_native), item);
        g_object_unref(item);
         */

        // main layout

        main.add_submenu("File", &file_submenu);
        main.add_submenu("Image", &image_submenu);
        main.add_submenu("Selection", &selection_submenu);
        main.add_submenu("Colors", &colors_submenu);
        main.add_submenu("Canvas", &canvas_submenu);
        main.add_submenu("Layers", &layer_submenu);
        main.add_submenu("Frames", &frame_submenu);
        main.add_submenu("Brushes", &brushes_submenu);
        main.add_submenu("Settings", &settings_submenu);
        main.add_submenu("Other", &other_submenu);
    }
}