//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/26/23
//

#pragma once

#include <mousetrap.hpp>
#include <app/app_component.hpp>
#include <app/app_signals.hpp>
#include <app/config_files.hpp>

namespace mousetrap
{
    namespace state::actions
    {
        DECLARE_GLOBAL_ACTION(resize_canvas_dialog, open);
    }

    class ResizeCanvasDialog : public AppComponent, public signals::LayerResolutionChanged
    {
        public:
            ResizeCanvasDialog();
            operator Widget*() override;

            void present();

        protected:
            void on_layer_resolution_changed() override;

        private:
            using Alignment = const char*;
            static constexpr Alignment TOP_LEFT = "top-left";
            static constexpr Alignment TOP = "top";
            static constexpr Alignment TOP_RIGHT = "top-right";
            static constexpr Alignment RIGHT = "right";
            static constexpr Alignment BOTTOM_RIGHT = "bottom-right";
            static constexpr Alignment BOTTOM = "bottom";
            static constexpr Alignment BOTTOM_LEFT = "bottom-left";
            static constexpr Alignment LEFT = "left";
            static constexpr Alignment CENTER = "center";

            Alignment _alignment = CENTER;
            void set_alignment(Alignment);

            class AlignmentSelector
            {
                public:
                    AlignmentSelector(ResizeCanvasDialog* owner);
                    ~AlignmentSelector();
                    operator Widget*();

                    void set_alignment(Alignment);

                private:
                    ResizeCanvasDialog* _owner;

                    struct ButtonAndArrow
                    {
                        ButtonAndArrow(Alignment alignment, const std::string& id);
                        operator Widget*();

                        Alignment _alignment;
                        ImageDisplay _arrow;
                        ToggleButton _button;
                    };

                    std::map<Alignment, ButtonAndArrow*> _arrows;

                    Box _top_row = Box(GTK_ORIENTATION_HORIZONTAL);
                    Box _center_row = Box(GTK_ORIENTATION_HORIZONTAL);
                    Box _bottom_row = Box(GTK_ORIENTATION_HORIZONTAL);

                    Box _all_rows = Box(GTK_ORIENTATION_VERTICAL);
                    Frame _frame;
            };

            AlignmentSelector _alignment_selector = AlignmentSelector(this);

            enum ScaleMode
            {
                ABSOLUTE = false,
                RELATIVE = true
            };

            void set_scale_mode(ScaleMode);
            ScaleMode _scale_mode = ABSOLUTE;

            void set_aspect_ratio_locked(bool);
            bool _aspect_ratio_locked = true;

            size_t _width;
            size_t _height;

            void set_width(float);
            void set_height(float);
            void set_x_offset(int);
            void set_y_offset(int);
            void set_final_size(size_t w, size_t h);

            Window _window;
            Dialog _dialog = Dialog(&_window, "Resize Canvas...");
            Box _window_box = Box(GTK_ORIENTATION_VERTICAL);

            SpinButton _width_spin_button;
            Label _width_label = Label("Width :");
            SeparatorLine _width_spacer;
            Box _width_box = Box(GTK_ORIENTATION_HORIZONTAL);

            SpinButton _height_spin_button;
            Label _height_label = Label("Height :");
            SeparatorLine _height_spacer;
            Box _height_box = Box(GTK_ORIENTATION_HORIZONTAL);

            SpinButton _x_offset_spin_button;
            Label _x_offset_label = Label("x :");
            SeparatorLine _x_offset_spacer;
            Box _x_offset_box = Box(GTK_ORIENTATION_HORIZONTAL);

            SpinButton _y_offset_spin_button;
            Label _y_offset_label = Label("y :");
            SeparatorLine _y_offset_spacer;
            Box _y_offset_box = Box(GTK_ORIENTATION_HORIZONTAL);

            Box _offset_box = Box(GTK_ORIENTATION_VERTICAL);

            Box _spin_button_box = Box(GTK_ORIENTATION_VERTICAL);
            Box _spin_button_and_dropdown_box = Box(GTK_ORIENTATION_HORIZONTAL);

            Label _instruction_label = Label(state::tooltips_file->get_value("resize_canvas_dialog", "instruction_label"));
            Label _final_size_label;
            Label _alignment_instruction_label = Label("<b>Alignment</b>");
            Label _alignment_state_label;
            Label _offset_instruction_label = Label("<b>Offset</b>");

            DropDown _absolute_or_relative_dropdown;
            Label _absolute_list_label = Label("px");
            Label _absolute_when_selected_label = Label("px");
            Label _relative_list_label = Label("%");
            Label _relative_when_selected_label = Label("%");

            CheckButton _maintain_aspect_ratio_button = CheckButton();
            Label _maintain_aspect_ratio_label;
            Box _maintain_aspect_ratio_box = Box(GTK_ORIENTATION_HORIZONTAL);

            Button _accept_button;
            Label _accept_button_label = Label("Resize");

            Button _cancel_button;
            Label _cancel_button_label = Label("Cancel");
    };

    namespace state
    {
        inline ResizeCanvasDialog* resize_canvas_dialog = nullptr;
    }
}
