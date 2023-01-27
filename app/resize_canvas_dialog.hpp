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
            operator Widget*() override;

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

            Alignment _alignment = CENTER;

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

                        Alignment _alignment;
                        ImageDisplay _arrow;
                        ToggleButton _button;
                    };

                    std::map<Alignment, ButtonAndArrow*> _arrows;

                    Box _top_row = Box(GTK_ORIENTATION_HORIZONTAL);
                    Box _center_row = Box(GTK_ORIENTATION_HORIZONTAL);
                    Box _bottom_row = Box(GTK_ORIENTATION_HORIZONTAL);

                    Box _main = Box(GTK_ORIENTATION_VERTICAL);
            };

            AlignmentSelector _alignment_selector = AlignmentSelector(this);
            Window _window;
            //Dialog _dialog = Dialog(&_window, "Resize Canvas...");
    };

    namespace state
    {
        inline ResizeCanvasDialog* resize_canvas_dialog = nullptr;
    }
}
