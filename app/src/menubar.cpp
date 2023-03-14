#include <app/menubar.hpp>
#include <app/bubble_log_area.hpp>
#include <app/brush_options.hpp>
#include <app/resize_canvas_dialog.hpp>
#include <app/scale_canvas_dialog.hpp>
#include <app/color_transform_dialog.hpp>

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

        auto initial_state = [](const std::string& group, const std::string& value) {
            return state::settings_file->get_value_as<bool>(group, value);
        };

        auto tooltip = [](const std::string& group, const std::string& value) {
            return state::tooltips_file->get_value_as<std::string>(group, value);
        };

        // FILE

        auto file_submenu = MenuModel();

        auto file_submenu_save_section = MenuModel();
        file_submenu_save_section.add_action(tooltip("save_file", "new_state"), save_file_new_state.get_id());
        file_submenu_save_section.add_action(tooltip("save_file", "open_load_dialog"), save_file_open_load_dialog.get_id());
        file_submenu_save_section.add_action(tooltip("save_file", "save_state_to_file"), save_file_save_state_to_file.get_id());
        file_submenu_save_section.add_action(tooltip("save_file", "open_save_dialog"), save_file_open_save_dialog.get_id());
        file_submenu_save_section.add_action(tooltip("save_file", "open_restore_from_backup_dialog"), save_file_open_restore_from_backup_dialog.get_id());
        file_submenu.add_section("Load / Save", &file_submenu_save_section);

        auto file_submenu_import_section = MenuModel();
        file_submenu_import_section.add_action(tooltip("save_file", "import_from_image"), save_file_import_from_image.get_id());
        file_submenu_import_section.add_action(tooltip("save_file", "export_as_image"), save_file_export_as_image.get_id());
        file_submenu_import_section.add_action(tooltip("save_file", "export_as_spritesheet"), save_file_export_as_spritesheet.get_id());
        file_submenu_import_section.add_action(tooltip("save_file", "export_metadata"), save_file_export_metadata.get_id());
        file_submenu.add_section("Import / Export", &file_submenu_import_section);

        auto file_submenu_close_section = MenuModel();
        file_submenu_close_section.add_action(tooltip("save_file", "safe_exit"), save_file_safe_exit.get_id());

        // SELECTION

        auto selection_submenu = MenuModel();


        auto selection_section = MenuModel();
        selection_section.add_action(tooltip("canvas", "select_all"), canvas_select_all.get_id());
        selection_section.add_action(tooltip("canvas", "invert_selection"), canvas_invert_selection.get_id());

        auto selection_mode_submenu = MenuModel();
        selection_mode_submenu.add_action(tooltip("canvas", "selection_mode_replace"), canvas_selection_mode_replace.get_id());
        selection_mode_submenu.add_action(tooltip("canvas", "selection_mode_add"), canvas_selection_mode_add.get_id());
        selection_mode_submenu.add_action(tooltip("canvas", "selection_mode_subtract"), canvas_selection_mode_subtract.get_id());

        auto selection_settings_section = MenuModel();
        selection_settings_section.add_stateful_action(tooltip("canvas", "selection_outline_animated"), canvas_selection_outline_animated.get_id(), true);
        selection_settings_section.add_stateful_action(tooltip("canvas", "allow_drawing_outside_selection"), canvas_toggle_allow_drawing_outside_selection.get_id(), false);
        selection_section.add_section("Settings", &selection_settings_section);

        auto selection_mode_section = MenuModel();
        selection_mode_section.add_submenu(tooltip("canvas", "selection_mode_submenu"), &selection_mode_submenu);
        selection_section.add_section("Mode", &selection_mode_section);

        selection_submenu.add_section("Selection", &selection_section);

        // COLORS

        auto colors_submenu = MenuModel();

        auto color_submenu_transform_section = MenuModel();
        color_submenu_transform_section.add_action(tooltip("color_transform_dialog", "invert"), color_transform_dialog_invert.get_id());
        color_submenu_transform_section.add_action(tooltip("color_transform_dialog", "open"), color_transform_dialog_open.get_id());
        color_submenu_transform_section.add_action("Replace... (TODO)", "TODO");
        color_submenu_transform_section.add_action("Palette Shift... (TODO)", "TODO");
        colors_submenu.add_section("Transform", &color_submenu_transform_section);

        auto palette_editing_section = MenuModel();
        palette_editing_section.add_stateful_action("Toggle Palette Locked", palette_view_toggle_palette_locked.get_id(), false);
        colors_submenu.add_section("Palette Editing", &palette_editing_section);

        auto color_palette_section = MenuModel();
        color_palette_section.add_action(tooltip("palette_view", "load"), palette_view_load.get_id());
        color_palette_section.add_action(tooltip("palette_view", "load_default"), palette_view_load_default.get_id());
        color_palette_section.add_action(tooltip("palette_view", "save"), palette_view_save.get_id());
        color_palette_section.add_action(tooltip("palette_view", "save_as"), palette_view_save_as.get_id());
        color_palette_section.add_action(tooltip("palette_view", "save_as_default"), palette_view_save_as_default.get_id());
        colors_submenu.add_section("Palette Load / Save", &color_palette_section);

        // LAYERS

        auto layer_submenu = MenuModel();

        auto layer_submenu_create_section = MenuModel();
        layer_submenu_create_section.add_action(tooltip("layer_view", "layer_new_above_current"), layer_view_layer_new_above_current.get_id());
        layer_submenu_create_section.add_action(tooltip("layer_view", "layer_new_below_current"), layer_view_layer_new_below_current.get_id());
        layer_submenu_create_section.add_action(tooltip("layer_view", "layer_create_from_visible"), layer_view_layer_create_from_visible.get_id());
        layer_submenu_create_section.add_action(tooltip("layer_view", "layer_duplicate"), layer_view_layer_duplicate.get_id());
        layer_submenu_create_section.add_action(tooltip("layer_view", "layer_delete"), layer_view_layer_delete.get_id());
        layer_submenu.add_section("Create / Delete", &layer_submenu_create_section);

        auto layer_submenu_merge_section = MenuModel();
        layer_submenu_merge_section.add_action(tooltip("layer_view", "layer_merge_down"), layer_view_layer_merge_down.get_id());
        layer_submenu_merge_section.add_action(tooltip("layer_view", "layer_flatten_all"), layer_view_layer_flatten_all.get_id());
        layer_submenu.add_section("Merge", &layer_submenu_merge_section);

        auto layer_move_section = MenuModel();
        layer_move_section.add_action(tooltip("layer_view", "layer_move_down"), layer_view_layer_move_down.get_id());
        layer_move_section.add_action(tooltip("layer_view", "layer_move_up"), layer_view_layer_move_up.get_id());
        layer_submenu.add_section("Layer Position", &layer_move_section);

        auto layer_property_section = MenuModel();
        layer_property_section.add_stateful_action(tooltip("layer_view", "toggle_layer_visible"), layer_view_toggle_layer_visible.get_id(), true);
        layer_property_section.add_stateful_action(tooltip("layer_view", "toggle_layer_locked"), layer_view_toggle_layer_locked.get_id(), true);
        layer_submenu.add_section("Layer Properties", &layer_property_section);

        auto layer_show_section = MenuModel();
        layer_show_section.add_action(tooltip("layer_view", "hide_all_other_layers"), layer_view_hide_all_other_layers.get_id());
        layer_show_section.add_action(tooltip("layer_view", "show_all_other_layers"), layer_view_show_all_other_layers.get_id());
        layer_submenu.add_section("Hide / Show", &layer_show_section);

        // FRAMES

        auto frame_submenu = MenuModel();

        auto frame_create_section = MenuModel();
        frame_create_section.add_action(tooltip("frame_view", "frame_new_right_of_current"), frame_view_frame_new_right_of_current.get_id());
        frame_create_section.add_action(tooltip("frame_view", "frame_new_left_of_current"), frame_view_frame_new_left_of_current.get_id());
        frame_create_section.add_action(tooltip("frame_view", "frame_delete"), frame_view_frame_delete.get_id());
        frame_submenu.add_section("Create / Delete", &frame_create_section);

        auto frame_move_section = MenuModel();
        frame_move_section.add_action(tooltip("frame_view", "frame_move_left"), frame_view_frame_move_left.get_id());
        frame_move_section.add_action(tooltip("frame_view", "frame_move_right"), frame_view_frame_move_right.get_id());
        frame_submenu.add_section("Frame Position", &frame_move_section);

        auto frame_navigation_section = MenuModel();
        frame_navigation_section.add_action(tooltip("frame_view", "toggle_playback_active"), frame_view_toggle_playback_active.get_id());
        frame_navigation_section.add_action(tooltip("frame_view", "go_to_previous_frame"), frame_view_go_to_previous_frame.get_id());
        frame_navigation_section.add_action(tooltip("frame_view", "go_to_next_frame"), frame_view_go_to_next_frame.get_id());
        frame_navigation_section.add_action(tooltip("frame_view", "jump_to_start"), frame_view_jump_to_start.get_id());
        frame_navigation_section.add_action(tooltip("frame_view", "jump_to_end"), frame_view_jump_to_end.get_id());
        frame_submenu.add_section("Playback / Navigation", &frame_navigation_section);

        auto cells_section = MenuModel();

        auto copy_to_cell_section = MenuModel();
        copy_to_cell_section.add_action(tooltip("frame_view", "copy_to_cell_above"), frame_view_copy_to_cell_above.get_id());
        copy_to_cell_section.add_action(tooltip("frame_view", "copy_to_cell_below"), frame_view_copy_to_cell_below.get_id());
        copy_to_cell_section.add_action(tooltip("frame_view", "copy_to_cell_before"), frame_view_copy_to_cell_before.get_id());
        copy_to_cell_section.add_action(tooltip("frame_view", "copy_to_cell_after"), frame_view_copy_to_cell_after.get_id());
        cells_section.add_submenu("Copy Cells", &copy_to_cell_section);

        auto swap_with_cell_section = MenuModel();
        swap_with_cell_section.add_action(tooltip("frame_view", "swap_with_cell_above"), frame_view_swap_with_cell_above.get_id());
        swap_with_cell_section.add_action(tooltip("frame_view", "swap_with_cell_below"), frame_view_swap_with_cell_below.get_id());
        swap_with_cell_section.add_action(tooltip("frame_view", "swap_with_cell_before"), frame_view_swap_with_cell_before.get_id());
        swap_with_cell_section.add_action(tooltip("frame_view", "swap_with_cell_after"), frame_view_swap_with_cell_after.get_id());
        cells_section.add_submenu("Swap Cells", &swap_with_cell_section);

        frame_submenu.add_section("Cells", &cells_section);

        auto frame_other_section = MenuModel();
        frame_other_section.add_stateful_action(tooltip("frame_view", "toggle_current_frame_is_keyframe"), frame_view_toggle_current_frame_is_keyframe.get_id(), false);
        frame_other_section.add_stateful_action(tooltip("frame_view", "toggle_onionskin_visible"), frame_view_toggle_onionskin_visible.get_id(), initial_state("frame_view", "onionskin_visible"));
        frame_submenu.add_section("Other", &frame_other_section);

        // CANVAS

        auto canvas_submenu = MenuModel();

        auto canvas_submenu_size_section = MenuModel();
        canvas_submenu_size_section.add_action(tooltip("resize_canvas_dialog", "open"), resize_canvas_dialog_open.get_id());
        canvas_submenu_size_section.add_action(tooltip("scale_canvas_dialog", "open"), scale_canvas_dialog_open.get_id());
        canvas_submenu.add_section("Size", &canvas_submenu_size_section);

        auto canvas_transform_section = MenuModel();
        canvas_transform_section.add_action(tooltip("image_transform_dialog", "open"), image_transform_dialog_open.get_id());
        canvas_transform_section.add_action(tooltip("image_transform_dialog", "rotate_clockwise"), image_transform_dialog_rotate_clockwise.get_id());
        canvas_transform_section.add_action(tooltip("image_transform_dialog", "rotate_counterclockwise"), image_transform_dialog_rotate_counterclockwise.get_id());
        canvas_submenu.add_section("Transform", &canvas_transform_section);

        auto canvas_grid_section = MenuModel();
        canvas_grid_section.add_stateful_action(tooltip("canvas", "toggle_grid_visible"), canvas_toggle_grid_visible.get_id(), initial_state("canvas", "grid_visible"));
        canvas_grid_section.add_action(tooltip("canvas", "open_grid_color_picker"), canvas_open_grid_color_picker.get_id());
        canvas_submenu.add_section("Grid", &canvas_grid_section);

        auto canvas_brush_section = MenuModel();
        canvas_brush_section.add_stateful_action(tooltip("canvas", "toggle_brush_outline_visible"), canvas_toggle_brush_outline_visible.get_id(), initial_state("canvas", "brush_outline_visible"));
        canvas_submenu.add_section("Brush", &canvas_brush_section);

        auto canvas_mirror_section = MenuModel();
        canvas_mirror_section.add_stateful_action(tooltip("canvas", "toggle_horizontal_symmetry_active"), canvas_toggle_horizontal_symmetry_active.get_id(), initial_state("canvas", "horizontal_symmetry_active"));
        canvas_mirror_section.add_stateful_action(tooltip("canvas", "toggle_vertical_symmetry_active"), canvas_toggle_vertical_symmetry_active.get_id(), initial_state("canvas", "vertical_symmetry_active"));
        canvas_mirror_section.add_action(tooltip("canvas", "open_symmetry_color_picker"), canvas_open_symmetry_color_picker.get_id(), false);
        canvas_submenu.add_section("Symmetry", &canvas_mirror_section);

        auto canvas_background_section = MenuModel();
        canvas_background_section.add_stateful_action(tooltip("canvas", "toggle_background_visible"), canvas_toggle_background_visible.get_id(), initial_state("canvas", "background_visible"));
        canvas_submenu.add_section("Background", &canvas_background_section);

        auto canvas_perspective_section = MenuModel();
        canvas_perspective_section.add_action(tooltip("canvas", "reset_transform"), canvas_reset_transform.get_id());
        canvas_submenu.add_section("View", &canvas_perspective_section);

        // BRUSHES

        auto brushes_submenu = MenuModel();
        auto brush_add_section = MenuModel();
        brush_add_section.add_action(tooltip("brush_options", "add_brush"), brush_options_add_brush.get_id());
        brush_add_section.add_action(tooltip("brush_options", "remove_brush"), brush_options_remove_brush.get_id());
        brushes_submenu.add_section("Add / Remove", &brush_add_section);

        auto brush_default_section = MenuModel();
        brush_default_section.add_action(tooltip("brush_options", "load_default_brushes"), brush_options_load_default_brushes.get_id());
        brush_default_section.add_action(tooltip("brush_options", "open_default_brush_directory"), brush_options_open_default_brush_directory.get_id());
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
        main.add_submenu("Canvas", &canvas_submenu);
        main.add_submenu("Selection", &selection_submenu);
        main.add_submenu("Colors", &colors_submenu);
        main.add_submenu("Layers", &layer_submenu);
        main.add_submenu("Frames", &frame_submenu);
        main.add_submenu("Brushes", &brushes_submenu);
        main.add_submenu("Settings", &settings_submenu);
        main.add_submenu("Other", &other_submenu);
    }
}