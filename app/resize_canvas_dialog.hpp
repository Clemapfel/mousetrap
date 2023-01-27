//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/26/23
//

#pragma once

#include <mousetrap.hpp>
#include <app/app_component.hpp>

namespace mousetrap
{
    class ResizeCanvasDialog : public AppComponent
    {
        public:
            ResizeCanvasDialog();
            operator Widget*();

            void present();

        private:
            enum Alignment
            {
                TOP_LEFT,
                TOP,
                TOP_RIGHT,
                RIGHT,
                BOTTOM_RIGHT,
                BOTTOM,
                BOTTOM_LEFT,
                LEFT,
                CENTER
            };

            Alignment _alignment;

            class AlignmentSelector
            {
                public:
                    AlignmentSelector(ResizeCanvasDialog* owner);
                    operator Widget*();

                private:
                    ResizeCanvasDialog* _owner;

                    struct ButtonAndArrow
                    {
                        ButtonAndArrow(Alignment alignment, const std::string& id);
                        operator Widget*();

                        Alignment alignment;
                        ImageDisplay arrow;
                        ToggleButton button;
                    };

                    std::map<Alignment, ButtonAndArrow> _arrows {
                        {TOP_LEFT, ButtonAndArrow(TOP_LEFT, "top_left")},
                        {TOP, ButtonAndArrow(TOP, "top")},
                        {TOP_RIGHT, ButtonAndArrow(TOP_RIGHT, "top_right")},
                        {RIGHT, ButtonAndArrow(RIGHT, "right")},
                        {BOTTOM_RIGHT, ButtonAndArrow(BOTTOM_RIGHT, "bottom_right")},
                        {BOTTOM, ButtonAndArrow(BOTTOM, "bottom")},
                        {BOTTOM_LEFT, ButtonAndArrow(BOTTOM_LEFT, "bottom_left")},
                        {LEFT, ButtonAndArrow(LEFT, "left")},
                        {CENTER, ButtonAndArrow(CENTER, "center")}
                    };

                    Box _top_row = Box(GTK_ORIENTATION_HORIZONTAL);
                    Box _center_row = Box(GTK_ORIENTATION_HORIZONTAL);
                    Box _bottom_row = Box(GTK_ORIENTATION_HORIZONTAL);

                    Box _main = Box(GTK_ORIENTATION_VERTICAL);
            };

            AlignmentSelector _alignment_selector = AlignmentSelector(this);
            Window _window;
            Dialog _dialog = Dialog(&_window, "Resize Canvas...");
    };

    namespace state
    {
        inline ResizeCanvasDialog* resize_canvas_dialog = nullptr;
    }
}
