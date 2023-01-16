//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), 1/6/23
//

#pragma once

#include <mousetrap.hpp>
#include <app/global_state.hpp>
#include <app/animation_preview.hpp>
#include <app/brush_options.hpp>
#include <app/frame_view.hpp>
#include <app/layer_view.hpp>
#include <app/palette_view.hpp>

namespace mousetrap
{
    namespace state::actions
    {
        inline Action save_file_new_state_action = Action("save_file.new_state");
        inline Action save_file_safe_exit_action = Action("save_file.safe_exit_action");

        inline Action save_file_save_state_to_file_action = Action("save_file.export_state_to_file");
        inline Action save_file_load_state_from_file_action = Action("save_file.load_state_from_file");

        inline Action save_file_open_save_dialog_action = Action("save_file.open_save_dialog");
        inline Action save_file_open_load_dialog_action = Action("save_file.open_load_dialog");
        inline Action save_file_open_restore_from_backup_dialog_action = Action("save_file.open_restore_from_backup_dialog_action");

        inline Action save_file_export_as_image_action = Action("save_file.export_as_image");
        inline Action save_file_export_as_spritesheet_action = Action("save_file.export_as_spritesheet");
        inline Action save_file_export_metadata_action = Action("save_file.export_metadata");

        inline Action save_file_import_from_image_action = Action("save_file.import_from_image");
    }

    KeyFile export_state_to_file();
    void import_state_from_file(KeyFile& in);
}
