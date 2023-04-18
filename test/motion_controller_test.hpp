//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/12/23
//

#pragma once

#include <mousetrap.hpp>

namespace mousetrap
{
    class MotionControllerTest : public Widget
    {
        private:
            Box main = Box(Orientation::VERTICAL);
            Separator area;
            Overlay overlay;
            Label label;
            Fixed fixed;

            Frame area_frame;

            MotionEventController motion_controller;

            void update_label(double x, double y)
            {
                std::stringstream str;
                str << "<small>" << int(x) << " | " << int(y) << "</small>";

                label.set_text(str.str());
                fixed.set_child_position(label, Vector2f(x, y));
            }

        public:
            MotionControllerTest()
            {
                motion_controller.connect_signal_motion_enter([](MotionEventController*, double x, double y, MotionControllerTest* instance){
                    instance->label.set_visible(true);
                    instance->update_label(x, y);
                }, this);

                motion_controller.connect_signal_motion([](MotionEventController*, double x, double y, MotionControllerTest* instance){
                    instance->update_label(x, y);
                }, this);

                motion_controller.connect_signal_motion_leave([](MotionEventController*, MotionControllerTest* instance){
                    instance->label.set_visible(false);
                }, this);

                main.add_controller(motion_controller);

                area.set_expand(true);
                area_frame.set_child(area);
                area_frame.set_expand(true);

                overlay.set_child(area_frame);
                overlay.add_overlay(fixed, false, true);
                overlay.set_margin(10);
                overlay.set_hide_on_overflow(true);
                fixed.add_child(label, {0, 0});

                main.push_back(overlay);
            }

            operator NativeWidget() const override
            {
                return main.operator NativeWidget();
            }
    };
}
