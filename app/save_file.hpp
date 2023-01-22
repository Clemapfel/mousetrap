//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), 1/6/23
//

#pragma once

#include <mousetrap.hpp>
#include <app/project_state.hpp>
#include <app/animation_preview.hpp>
#include <app/frame_view.hpp>
#include <app/layer_view.hpp>
#include <app/palette_view.hpp>

namespace mousetrap
{
    namespace state::actions
    {
        DECLARE_GLOBAL_ACTION(save_file, new_state);
        DECLARE_GLOBAL_ACTION(save_file, safe_exit);

        DECLARE_GLOBAL_ACTION(save_file, save_state_to_file);
        DECLARE_GLOBAL_ACTION(save_file, load_state_from_file);

        DECLARE_GLOBAL_ACTION(save_file, open_save_dialog);
        DECLARE_GLOBAL_ACTION(save_file, open_load_dialog);
        DECLARE_GLOBAL_ACTION(save_file, open_restore_from_backup_dialog);

        DECLARE_GLOBAL_ACTION(save_file, export_as_image);
        DECLARE_GLOBAL_ACTION(save_file, export_as_spritesheet);
        DECLARE_GLOBAL_ACTION(save_file, export_metadata);

        DECLARE_GLOBAL_ACTION(save_file, import_from_image);
    }

    KeyFile export_state_to_file();
    void import_state_from_file(KeyFile& in);
}
