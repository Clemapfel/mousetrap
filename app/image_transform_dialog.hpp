//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/14/23
//

#pragma once

#include <app/project_state.hpp>
#include <mousetrap.hpp>
#include <app/app_component.hpp>
#include <app/config_files.hpp>

namespace mousetrap
{
    namespace state::actions
    {
        DECLARE_GLOBAL_ACTION(image_transform_dialog, open);

        DECLARE_GLOBAL_ACTION(image_transform_dialog, flip_horizontally);
        DECLARE_GLOBAL_ACTION(image_transform_dialog, flip_vertically);
        DECLARE_GLOBAL_ACTION(image_transform_dialog, rotate_clockwise);
        DECLARE_GLOBAL_ACTION(image_transform_dialog, rotate_counterclockwise);
    }

    class ImageTransformDialog : public AppComponent
    {
        public:
            ImageTransformDialog();
            operator Widget*() override;

            void present();

        private:
            void update_preview();
            void reset();

            Window _window;
            Dialog _dialog = Dialog(&_window, "Canvas Transform...");
            Box _window_box = Box(GTK_ORIENTATION_VERTICAL);

            bool _flip_horizontally = false;
            bool _flip_vertically = false;

            Label _instruction_label = Label("<b>Flip Canvas...</b>");

            Label _h_flip_label = Label("Flip Horizontally:");
            SeparatorLine _h_flip_spacer;
            CheckButton _h_flip_check_button;
            Box _h_flip_box = Box(GTK_ORIENTATION_HORIZONTAL);
            void set_flip_horizontally(bool);

            Label _v_flip_label = Label("Flip Vertically:");
            SeparatorLine _v_flip_spacer;
            CheckButton _v_flip_check_button;
            Box _v_flip_box = Box(GTK_ORIENTATION_HORIZONTAL);
            void set_flip_vertically(bool);

            Label _apply_to_label = Label("<b>Apply To...</b>");

            DropDown _apply_to_dropdown;
            Label _current_layer_list_label = Label(apply_scope_to_string(ApplyScope::CURRENT_LAYER));
            Label _current_layer_when_selected_label = Label(apply_scope_to_string(ApplyScope::CURRENT_LAYER));
            Label _current_frame_list_label = Label(apply_scope_to_string(ApplyScope::CURRENT_FRAME));
            Label _current_frame_when_selected_label = Label(apply_scope_to_string(ApplyScope::CURRENT_FRAME));
            Label _current_cell_list_label = Label(apply_scope_to_string(ApplyScope::CURRENT_CELL));
            Label _current_cell_when_selected_label = Label(apply_scope_to_string(ApplyScope::CURRENT_CELL));
            Label _everywhere_list_label = Label(apply_scope_to_string(ApplyScope::EVERYWHERE));
            Label _everywhere_when_selected_label = Label(apply_scope_to_string(ApplyScope::EVERYWHERE));
            Label _selection_list_label = Label(apply_scope_to_string(ApplyScope::SELECTION));
            Label _selection_when_selected_label = Label(apply_scope_to_string(ApplyScope::SELECTION));

            Label _accept_label = Label("Apply");
            Button _accept_button;

            Label _cancel_label = Label("Cancel");
            Button _cancel_button;

            SeparatorLine _button_box_spacer;
            Box _button_box = Box(GTK_ORIENTATION_HORIZONTAL);

            static void test()
            {
                active_state->rotate_clockwise();
            }
    };

    namespace state
    {
        inline ImageTransformDialog* image_transform_dialog = nullptr;
    }
}
