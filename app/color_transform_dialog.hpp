//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 2/5/23
//

#pragma once

#include <mousetrap.hpp>
#include <app/app_component.hpp>
#include <app/config_files.hpp>

namespace mousetrap
{
    namespace state::actions
    {
        DECLARE_GLOBAL_ACTION(color_transform_dialog, open);
    }

    class ColorTransformDialog : public AppComponent
    {
        public:
            ColorTransformDialog();
            operator Widget*() override;

            void present();

        private:
            void update_preview();

            Window _window;
            Dialog _dialog = Dialog(&_window, "Color Transform...");
            Box _window_box = Box(GTK_ORIENTATION_VERTICAL);

            Label _offset_label = Label("<b>Offset</b>");
            Label _offset_instruction_label = state::tooltips_file->get_value("color_transform_dialog", "offset_instruction_label");

            float _h_offset = 0;
            void set_h_offset(float);
            Label _h_label = Label("Hue:");
            Scale _h_offset_scale = Scale(-1, 1, 0.001);
            SeparatorLine _h_offset_spacer;
            Box _h_offset_box = Box(GTK_ORIENTATION_HORIZONTAL);

            float _s_offset = 0;
            void set_s_offset(float);
            Label _s_label = Label("Value:");
            Scale _s_offset_scale = Scale(-1, 1, 0.001);
            SeparatorLine _s_offset_spacer;
            Box _s_offset_box = Box(GTK_ORIENTATION_HORIZONTAL);

            float _v_offset = 0;
            void set_v_offset(float);
            Label _v_label = Label("Saturation:");
            Scale _v_offset_scale = Scale(-1, 1, 0.001);
            SeparatorLine _v_offset_spacer;
            Box _v_offset_box = Box(GTK_ORIENTATION_HORIZONTAL);

            float _r_offset = 0;
            void set_r_offset(float);
            Label _r_label = Label("Red:");
            Scale _r_offset_scale = Scale(-1, 1, 0.001);
            SeparatorLine _r_offset_spacer;
            Box _r_offset_box = Box(GTK_ORIENTATION_HORIZONTAL);

            float _g_offset = 0;
            void set_g_offset(float);
            Label _g_label = Label("Green:");
            Scale _g_offset_scale = Scale(-1, 1, 0.001);
            SeparatorLine _g_offset_spacer;
            Box _g_offset_box = Box(GTK_ORIENTATION_HORIZONTAL);

            float _b_offset = 0;
            void set_b_offset(float);
            Label _b_label = Label("Blue:");
            Scale _b_offset_scale = Scale(-1, 1, 0.01);
            SeparatorLine _b_offset_spacer;
            Box _b_offset_box = Box(GTK_ORIENTATION_HORIZONTAL);

            float _a_offset = 0;
            void set_a_offset(float);
            Label _a_label = Label("Opacity:");
            Scale _a_offset_scale = Scale(-1, 1, 0.01);
            SeparatorLine _a_offset_spacer;
            Box _a_offset_box = Box(GTK_ORIENTATION_HORIZONTAL);

            Label _apply_to_label = Label("<b>Apply To...</b>");

            using ApplyToTarget = const char*;
            static constexpr ApplyToTarget CURRENT_LAYER = "Current Layer";
            static constexpr ApplyToTarget CURRENT_FRAME = "Current Frame";
            static constexpr ApplyToTarget CURRENT_CELL = "Current Cell";
            static constexpr ApplyToTarget EVERYWHERE = "All Layers and Frames";
            ApplyToTarget _target = EVERYWHERE;

            DropDown _apply_to_dropdown;
            Label _current_layer_list_label = Label(CURRENT_LAYER);
            Label _current_layer_when_selected_label = Label(CURRENT_LAYER);
            Label _current_frame_list_label = Label(CURRENT_FRAME);
            Label _current_frame_when_selected_label = Label(CURRENT_FRAME);
            Label _current_cell_list_label = Label(CURRENT_CELL);
            Label _current_cell_when_selected_label = Label(CURRENT_CELL);
            Label _everywhere_list_label = Label(EVERYWHERE);
            Label _everywhere_when_selected_label = Label(EVERYWHERE);

            SeparatorLine _button_spacer;

            Label _accept_label = Label("Apply");
            Button _accept_button;

            Label _cancel_label = Label("Cancel");
            Button _cancel_button;

            Label _reset_label = Label("Reset");
            Button _reset_button;

            Box _button_box = Box(GTK_ORIENTATION_HORIZONTAL);
            SeparatorLine _button_box_spacer;
    };

    namespace state
    {
        inline ColorTransformDialog* color_transform_dialog = nullptr;
    }
}
